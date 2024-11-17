/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"
#include<stdio.h>
#include<string.h>


struct jitc {
    void *sharedFile; 
};

int jitc_compile(const char *input, const char *output){
    if (!input || !output) {
        return -1; /*Error: invalid arguments*/
    }

    char *command[] = {"gcc","-fPIC","-shared","-o", (char*) output, (char*) input, NULL};

    int pid = fork();
    if(pid < 0){
    	TRACE(0);
    	return -1;
    }
    if(pid == 0) {
    	execv("/usr/bin/gcc", command);
    	perror("execv failed");
    } else {
    	int status;
    	waitpid(pid, &status, 0);
    	if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    		return 0;
    	} else {
    		printf("Compilation failed %d", WEXITSTATUS(status));
    		return -1;
    	}
    }
    return 0; // Success
}

struct jitc *jitc_open(const char *pathname)
{
    void *sharedLib = dlopen(pathname, RTLD_LAZY);
    struct jitc *jitc = malloc(sizeof(struct jitc));
    if (!sharedLib) {
        
        fprintf(stderr, "Could not load library: %s\n", dlerror());
        return NULL;
    }
    if(!jitc) {
    	printf("Could not allocate memory");
    	dlclose(sharedLib);
    	return NULL;
    }
    jitc->sharedFile = sharedLib;
    return jitc;
}

/* research the above Needed API and design accordingly */
void jitc_close(struct jitc *jitc) {
    if (jitc == NULL) {
        return;  // Nothing to do
    }

    if (jitc->sharedFile != NULL) {
        dlclose(jitc->sharedFile);  // Unload the module
    }

    free(jitc);
}

long jitc_lookup(struct jitc *jitc, const char *symbol) {
    void *addr = dlsym(jitc->sharedFile, symbol);
    if(!addr) {
    	printf("No address");
    	return 0;
    }
    return (long) addr;
}

/* research the above Needed API and design accordingly */
