#ifndef T_H
#define T_T

//Globals ************
PROC *running;              //current executing PROC
PROC *freeList;             //list of FREE PROCs
PROC *readyQueue;           //priority queue of READY PROCs
PROC *sleepQueue;

PROC proc[NPROC];

//Functions ************
int body();
int set_vector(u16 segment, u16 handler);
PROC *kfork(char *filename);

//Assembly Functions **********
int int80h();

#endif
