#include "Queue.h"


//displays the process id and the priority on the queue
int printQueue(char *name, PROC *queue)
{
  //set the procPtr to the front of the list
  PROC *procPtr = queue;

  printf("%s: ", name);

  //go until the end of the list
  while(procPtr)
  {
    printf("[P%d, priority: %d] -> ", procPtr->pid, procPtr->priority);
    procPtr = procPtr->next;
  }

  printf("null \n");
}

//prints lists displays the process id and the next 
int printList(char *name, PROC *list)
{
  //set the procPtr to the front of the list
  PROC *procPtr = list;

  printf("%s: ", name);

  //go until the end of the list
  while(procPtr)
  {
    printf("P%d -> ", procPtr->pid);
    procPtr = procPtr->next;
  }

  printf("null \n");
}

//displays the process id and the priority on the queue
int printSleepQueue(char *name, PROC *queue)
{
  //set the procPtr to the front of the list
  PROC *procPtr = queue;

  printf("%s: ", name);

  //go until the end of the list
  while(procPtr)
  {
    printf("[P%d, event: %d] -> ", procPtr->pid, procPtr->event);
    procPtr = procPtr->next;
  }

  printf("null \n");
}

//enques into a queue structure. 
int enqueue(PROC **queue, PROC *tmpProc)
{
  PROC *tmpPtr = *queue;

  // if the list is empty
  if(!tmpPtr)
  {
    *queue = tmpProc;
    tmpProc->next = 0;
    return 1;
  }

  //checks the first item in the que
  if(tmpPtr->priority < tmpProc->priority)
  {
    *queue = tmpProc;
    tmpProc->next = tmpPtr;
  
    return 1;
  }

  //while there is stuff left in the list
  while(tmpPtr->next)
  {
   // printf("Enqueu: pid = %d\n", tmpPtr->pid);
    //if we have found the point we want to add the item
    if((tmpPtr->next)->priority < tmpProc->priority)
    {
      break;
    }
    else
    {
      tmpPtr = tmpPtr->next;  
    }
  }

  //assign the new back
  tmpProc->next = tmpPtr->next;
  tmpPtr->next = tmpProc;

  return 1;
}

//gets the first item of the queue and returns it
PROC *dequeue(PROC **queue)
{
  
  PROC *tmpPtr = *queue;
  
  if(!tmpPtr)
  {
    printf("ERROR: dequeue failed empty queue\n");
    return 0;
  }

  *queue = tmpPtr->next;
  tmpPtr->next = 0;
  
  return tmpPtr;
}

//currently calles dequeue
PROC *getProc(PROC **list)
{
  return dequeue(list);
}

//puts a proc at the back of the list
int putProc(PROC **list, PROC *tmpProc)
{
  PROC *tmpPtr = *list;

  // if the list is empty
  if(!tmpPtr)
  {
    tmpProc->next = tmpPtr;
    tmpPtr = tmpProc;
    return 1;
  }

  //go to the back
  while(tmpPtr->next)
  {
    tmpPtr = tmpPtr->next;
  } 

  //assign the new back
  tmpProc->next = tmpPtr->next;
  tmpPtr->next = tmpProc;

  return 1;
}
