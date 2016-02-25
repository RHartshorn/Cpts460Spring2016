#ifndef QUEUE_H
#define QUEUE_H

#include "type.h"

int printQueue(char *name, PROC *list);
int printList(char *name, PROC *queue);
int printSleepQueue(char *name, PROC *queue);

int enqueue(PROC **queue, PROC *tmpProc);
PROC *dequeue(PROC **queue);

PROC *getProc(PROC **list);
int putProc(PROC **list, PROC *tmpProc);

#endif