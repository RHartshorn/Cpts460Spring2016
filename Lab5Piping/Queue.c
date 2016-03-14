#include "Globals.h"
#include "Header.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

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