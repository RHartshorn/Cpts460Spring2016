#ifndef KERNEL_H
#define KERNEL_H

#include "type.h"

extern PROC proc[];
extern PROC *running, *freeList, *readyQueue, *sleepList;

PROC *kfork(char *filename);
int ksleep(int eventValue);
int kwakeup(int eventValue);
int kexit(int exitValue);
int kwait(int *status);

//utilityFunctions
int activeProcCount();
int hasChild(int parrentPid);

#endif