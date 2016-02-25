#ifndef T_H
#define T_H

//**************************************************
//  bio.o, queue.o loader.o are in mtxlib
//**************************************************
//#include "bio.c" 
//#include "queue.c" 
//#include "loader.c" 

#include "type.h"
#include "Queue.h"
#include "Kernel.h" 

//#include "wait.c"             // YOUR wait.c   file
//#include "int.c"              // YOUR int.c    file

PROC proc[NPROC];
PROC *running, *freeList, *readyQueue, *sleepList;

int procSize = sizeof(PROC);
int nproc = 0;

char *pname[] = {"Sun", "Mercury", "Venus", 
                 "Earth",  "Mars", "Jupiter", 
                 "Saturn", "Uranus", "Neptune"};
int color;

int int80h(); //In assembly code

#endif