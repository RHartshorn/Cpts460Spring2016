#ifndef QUEUE_H
#define QUEUE_H

int enqueue(PROC **queue, PROC *tmpProc);
PROC *dequeue(PROC **queue);
PROC *getProc(PROC **list);
int putProc(PROC **list, PROC *tmpProc);

#endif