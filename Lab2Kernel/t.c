#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define RED 

#define FREE 0
#define READY 1
#define SLEEP 2
#define BLOCK 3
#define ZOMBIE 4

extern int color;

//dont move around ksp next or kstack
typedef struct proc
{
    struct proc *next;
    int *ksp;               // saved ksp when not running
    struct proc *parent;    //  0    1     2     3     4 
    int status;             //Free|Ready|Sleep|Block|Zombie
    int priority;
    int pid;                // add pid for identify the proc
    int ppid;

    int kstack[SSIZE];      // proc stack area
}PROC;

int  procSize = sizeof(PROC);

PROC proc[NPROC];

PROC *running;              //current executing PROC
PROC *freeList;             //list of FREE PROCs
PROC *readyQueue;           //priority queue of READY PROCs

//gets the frist PROC off a list

int enqueue(PROC **queue, PROC *tmpProc)
{
  PROC *tmpPtr = *queue;

  // if the list is empty
  if(!tmpPtr)
  {
    printf("Enqueu: empty\n");
    *queue = tmpProc;
    tmpProc->next = 0;
    return 1;
  }

  if(tmpPtr->priority < tmpProc->priority)
  {
    *queue = tmpProc;
    tmpProc->next = tmpPtr;
  
    return 1;

  }

  //while there is stuff left in the list
  while(tmpPtr->next)
  {
    printf("Enqueu: pid = %d\n", tmpPtr->pid);
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

PROC *getProc(PROC **list)
{
  return dequeue(list);
  // PROC *tmpPtr = *queue;

  // if(!tmpPtr)
  // {
  //   printf("ERROR: getProc failed empty queue\n");
  //   return 0;
  // }

  // //while there is stuff left in the list
  // while(tmpPtr->next)
  // {
    
  //   //if we have found the point we want to add the item
  //   if((tmpPtr->next)->status == READY)
  //   {
  //     break;
  //   }

  //   tmpPtr = tmpPtr->next;  
  // }

  // //if nothing is ready
  // if(!tmpPtr->next)
  // {
  //   printf("ERROR: getProc failed nothing is READY\n");
  //   return 0;
  // }
  // //tmpPtr = tmpPtr->next;
  
}
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

int printQueue(char *name, PROC *queue)
{
  //set the procPtr to the front of the list
  PROC *procPtr = queue;

  printf("%s: ", name);

  //go until the end of the list
  while(procPtr)
  {
    printf("[P%d , pri: %d] -> ", procPtr->pid, procPtr->priority);
    procPtr = procPtr->next;
  }

  printf("null \n");

}

int printGlobals()
{
  printf("-------------globals-------------\n");
  printList("running", running);
  printList("freeList", freeList);
  printQueue("readyQueue", readyQueue);
  printf("---------------------------------\n");
}

int body()
{ 
   char c;
   printf("proc %d resumes to body()\n", running->pid);
   while(1)
   {
    printGlobals();
    color = running->pid + 7;
    printf("proc %d running : enter a key : ", running->pid);
    c = getc(); 
    printf("%c\n", c);
    
    switch(c)
    {
      case 'f' : do_kfork(); break;
      case 's' : do_tswitch(); break;
      case 'q' : do_exit(); break;
    }
   }
}

PROC *kfork()
{
  int i;
  PROC *tmpProc = getProc(&freeList);

  //if the proc is null
  if(!tmpProc)
  {
    printf("ERROR: kFork failed, empty PROC list.\n");
    return 0;
  }

  tmpProc->status = READY;
  tmpProc->priority = 1;
  tmpProc->ppid = running->pid;

  //clearing old kstack
  for(i = 1; i < 10; i++)
  {
    tmpProc->kstack[SSIZE - 9] = 0;
  }

  tmpProc->kstack[SSIZE - 1] = (int)body;
  tmpProc->ksp  = &tmpProc->kstack[SSIZE - 9];
  
  enqueue(&readyQueue, tmpProc);

  return tmpProc;
}

int do_kfork()
{
  kfork();
}
int do_tswitch()
{
  tswitch();
}
int do_exit()
{
  running->status = ZOMBIE;
  tswitch();
}

int init()
{
   PROC *tmpProc;
   int i;
   int j;

   /* initialize all proc's */
   for (i = 0; i < NPROC; i++)
   {
      
      tmpProc = &proc[i];
      tmpProc->status = FREE;
      tmpProc->pid = i;                        // pid = 0,1,2,..NPROC-1
      tmpProc->ppid = 0;
      //makes sure we dont go off the end of the proc array
      if(i == (NPROC - 1))
      {
        tmpProc->next = 0;
      }
      else
      {
        tmpProc->next = &proc[i+1];              // point to next proc
      }

      tmpProc->parent = 0;
  }

  readyQueue = 0;
  freeList = &proc[0];

  tmpProc = getProc(&freeList);
  tmpProc->pid = 0;
  tmpProc->status = READY;
  tmpProc->priority = 0;
  running = tmpProc;

  printf("init complete\n");

 }

int scheduler()
{
   if(running->status == READY)
   {
    printf("scheduler: enqueue on readyQueue\n");
    enqueue(&readyQueue, running);
   }

    printf("scheduler: dequeue on readyQueue\n");
    running = dequeue(&readyQueue);

}

main()
{
  char usrInput;

  printf("\n\nMTX starts in main()\n");
  init();
  
  printGlobals();

  while(1){
    
    printf("proc 0  running : enter a key : \n");
    usrInput = getc();
    kfork();
    printf("\n");

    //if readyQueue != empty
    if(readyQueue)
    {
      tswitch();
    }
  }
}