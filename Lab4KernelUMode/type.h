#ifndef TS_H
#define TS_H

#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define MAXLEN 1024

#define FREE 0
#define READY 1
#define SLEEP 2
#define BLOCK 3
#define ZOMBIE 4
#define RUNNING 5

typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char u8;

//dont move around ksp next or kstack
typedef struct proc{
    struct proc *next;
    int    *ksp;               // at offset 2 //kernal stack space 

    int    uss;                // user stack space // what segment the users stack is in
    int    usp;                // user stack pointer

    int    inkmode;            // at offset 8 // in k mode

    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];           // name string of PROC

    int    kstack[SSIZE];      // per proc stack area
}PROC;

extern int color;

char *statusArray[] = {"FREE", "READY", "SLEEP", "BLOCK", "ZOMBIE", "RUNNING"};

char *pname[] = {"Sun", "Mercury", "Venus", 
                 "Earth",  "Mars", "Jupiter", 
                 "Saturn", "Uranus", "Neptune"};

int  procSize = sizeof(PROC);

//Assembly Functions


#endif