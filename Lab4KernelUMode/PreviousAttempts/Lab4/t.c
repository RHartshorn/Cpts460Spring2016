#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define MAXLEN 1024

#define FREE 0
#define READY 1
#define SLEEP 2
#define BLOCK 3
#define ZOMBIE 4

#define BASE8 8;
#define BASE10 10;
#define BASE16 16;


typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char u8;

char *ascciTable = "0123456789ABCDEF";

extern int color;

int body();

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
    int event;              //sleep event
    int exitCode;           //exit code
    int kstack[SSIZE];      // proc stack area
}PROC;

int  procSize = sizeof(PROC);

PROC proc[NPROC];

PROC *running;              //current executing PROC
PROC *freeList;             //list of FREE PROCs
PROC *readyQueue;           //priority queue of READY PROCs
PROC *sleepQueue;

//******************************* PROC Functions **********************************

PROC *initProc(PROC *tmpProc, int pid)
{
  tmpProc->status = FREE;
  tmpProc->pid = pid;
  tmpProc->ppid = 0;
  tmpProc->event = 0;
  tmpProc->exitCode = 0;
  tmpProc->priority = 0;
  tmpProc->parent = 0;
}

//******************************* IO Functions **********************************
// int prints(char *str)
// {
//   char *strPtr = str;

//   while(*strPtr)
//   {
//     if(*strPtr == '\n')
//     {
//       putc('\n');
//       putc('\r');
//     }
//     else
//     {
//       putc(*strPtr);
//     }

//     strPtr++;
//   }
// }

// int printc(char c)
// {
//   putc('c');
// }

// int printu(u16 num, int base)
// {
//   char *c;

//   if(num == 0)
//   {
//     return 0;
//   }

//   c = ascciTable[num % base];
//   printdh(num / 10, base);
//   putc(*c);
// }

// int printl(u32 num, base)
// {
//   char c;
//   if(num < 0)
//   {
//     putc('-');
//     num *= (-1);
//   }

//   printd(num, base);
// }

// int printlh(u32 num, int base)
// {
//   char c;
//   if(num < 0)
//   {
//     putc('-');
//     num *= (-1);
//   }

//   printdh(num, base);
// }

// int printx(u16 num)
// {
//   putc('0');
//   putc('x');

//   printd(num);
// }

// int printf(char *tmp, ...)
// {
//   int flag = 0;
//   char *strPtr = tmp;
  
//   while(*strPtr != 0)
//   {
//     if(*strPtr == '%')
//     {
//       flag = 1;
//     }
//     else if(flag == 1)
//     {
//       switch(*strPtr)
//       {
//         case 'd' : printd(*ip, BASE10); break;
//         case 's' : prints(*ip); break;
//         case 'x' : printx(*ip); break; 
//         case 'u' : printu(*ip, BASE10); break;
//         case 'l' : printl(*ip, BASE10); ip++; break;  
//         case 'c' : printc(*ip); ((u8 *)ip)++; break; 
//       }

//       ip++;
//     }
//     putc(*strPtr);
//     strPtr++;
//   }
// }

char *gets(char str[])
{
  int len = 0;
  char c;
  char *strPtr = str;

  while((c = getc()) != '\r' && len < MAXLEN - 1)
  {
    *strPtr++ = c;
    putc(c);  //displays to screen
    len++;
  }

  putc('\n');
  putc('\r');

  *strPtr = 0;
  return str;
}

int atoi(char *str)
{
  int retVal = 0;
  int sign = 1;
  char *strPtr = str;

  if(*strPtr == '-')
  {
    sign = -1;
    strPtr++;
  }
  //go untill the end of the string
  while(*strPtr != 0)
  {
    retVal *= 10;
    retVal += (int)*strPtr - '0';
    
    strPtr++;
  }

  return retVal * sign;
}

//******************************* Queue & List Functions **********************************

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

//*******************************************************************************

int printStatuses()
{
  int i;
  PROC *tmpProc;

  printf("SLEEP = ");
  //go through all procs in the proc array 
  for(i = 0; i < NPROC; i++)
  {  
    tmpProc = &proc[i];
    if(tmpProc->status == SLEEP)
    {
      printf("[P%d, On: %d] -> ", tmpProc->pid, tmpProc->event);
    }
  }
  printf("null \n");

  printf("ZOMBIE = ");
  //go through all procs in the proc array 
  for(i = 0; i < NPROC; i++)
  {  
    tmpProc = &proc[i];
    if(tmpProc->status == ZOMBIE)
    {
      printf("[P%d, exit: %d] -> ", tmpProc->pid, tmpProc->exitCode);
    }
  }
  printf("null \n");

}
int printProcArray()
{
  int i;
  PROC *tmpProc;

  printf("**********PROC ARRAY************\n");
  
  for(i = 0; i < NPROC; i++)
  {
    tmpProc = &proc[i];
    printf("pid = %d\n", tmpProc->pid);
    printf("ppid = %d\n", tmpProc->ppid);
    printf("ksp = %d\n", tmpProc->ksp);
    printf("status = ");
    switch(tmpProc->status)
    {
      case READY  : printf("READY\n"); break;
      case FREE   : printf("FREE\n"); break;
      case SLEEP  : printf("SLEEP\n"); break;
      case BLOCK  : printf("BLOCK\n"); break;
      case ZOMBIE : printf("ZOMBIE\n"); break; 
    }
    printf("priority = %d\n", tmpProc->priority);
    printf("event = %d\n", tmpProc->event);
    printf("exitCode = %d\n", tmpProc->exitCode);
    printf("********************************\n");
    printf("press key to continue printing: ");
    getc();
    printf("\n");
  }
}

int printGlobals()
{
  printf("-------------globals-------------\n");
  //printProcArray();
  printList("running", running);
  printList("freeList", freeList);
  printQueue("readyQueue", readyQueue);
  //printSleepQueue("sleepQueue", sleepQueue);
  printStatuses();
  printf("---------------------------------\n");
}

int printMenu()
{
  printf("-------------Menu-------------\n");
  printf("f: kfork\n" );
  printf("s: tswitch\n" );
  printf("q: exit\n" );
  printf("z: ksleep\n" );
  printf("a: kwakeup\n" );
  printf("w: kwait\n" );
  printf("------------------------------\n");
}

int body()
{ 
   char c;
   printf("proc %d resumes to body()\n", running->pid);
   
   while(1)
   {
    color = running->pid + 7;
    
    printGlobals();
    printf("proc %d running : enter a key : ", running->pid);

    c = getc(); 
    printf("%c\n", c);
    
    switch(c)
    {
      case 'f' : do_kfork(); break;           //kfork a child
      case 's' : do_tswitch(); break;         //switch process
      case 'q' : do_exit(); break;            //make running proc a zombie
      case 'z' : do_ksleep(); break;          //running proc goes to sleep on an event value
      case 'a' : do_kwakeup(); break;         //wake all procs sleeping on event
      case 'w' : do_kwait();  break;          //wait for zombie child
      default  : printMenu(); break;
    }
   }
}

//**************************** Functions Called From Body **************************** 

//records the event value in the running proc's event then changes the running proc's status to SLEEP
//finaly it switches process
int ksleep(int eventValue)
{
  
  running->event = eventValue;
  running->status = SLEEP;
 
  //get a proc from the ready queue and make it run
  tswitch();
}

//goes through the proc array and changes all sleeping procs with the same eventValue
//to status READY and puts them in the ready queue
int kwakeup(int eventValue)
{

  int i;
  PROC *tmpProc;

  //go through all procs in the proc array 
  for(i = 0; i < NPROC; i++)
  {  

    tmpProc = &proc[i];

    //check to see if they are asleep and if they have the correct eventValue
    if(tmpProc->status == SLEEP && tmpProc->event == eventValue)
    {      
      //set the status to ready and the event to 0. add them to the ready queue
      tmpProc->event = 0;
      tmpProc->status = READY;

      enqueue(&readyQueue, tmpProc);
    }
  }

  return 1;
}

int activeProcCount()
{
  int i;
  int activeProcs = 0;
  PROC *tmpProc;
  
  for(i = 2; i < NPROC; i++)
  {
    
    tmpProc = &proc[i];

    if(tmpProc->status != FREE)
    {
      activeProcs++;
    }
  }
  printf("Number of activeProcs = %d\n", activeProcs);
  return activeProcs;
}

int kexit(int exitValue)
{
  PROC *tmpProc;
  int i;
  int wakeupP1 = 0;

  //if nproc = number of active procs
  if (running->pid == 1 && activeProcCount() > 0) 
  {
    printf("other procs still exist, p1 can't die yet\n");
    return -1;
  }

  //send children(dead or alive) to p1's orphanage
  for(i = 1; i < NPROC; i++)
  {
    tmpProc = &proc[i];

    if(tmpProc->status != FREE && tmpProc->ppid == running->pid)
    {
      tmpProc->ppid = 1;
      tmpProc->parent = &proc[1];
      wakeupP1++;
    }
  }

  //recorde exit value and become a zombie
  running->exitCode = exitValue;
  running->status = ZOMBIE;

  //wakeup parent and also p1 if necessary
  kwakeup(running->parent); //parrent sleeps on its proc address

  if(wakeupP1)
  {
    kwakeup(&proc[1]);
  }

  tswitch();
}

//given a pid of a parrent proc this checks if
//any procs have the parrent pid as its ppid
//returns 1 if true and 0 if false
int hasChild(int parrentPid)
{
  int i;
  PROC *tmpProc;
  
  for(i = 0; i < NPROC; i++)
  {
    
    tmpProc = &proc[i];
    
    if(tmpProc->ppid == parrentPid)
    {
      return 1;
    }
  }

  return 0;
}

int kwait(int *status)
{
  int childPid;
  int i;
  PROC *tmpProc;

  //caller is the running proc
  
  //if caller has no child (look through the proc list for procs with their ppid == running->pid)  
  if(!hasChild(running->pid))
  {
    printf("Error: The running P%d does not have children\n", running->pid);
    *status = -1;
    return -1;
  }  

  while(1)
  {
    for(i = 0; i < NPROC; i++)
    {
    
      tmpProc = &proc[i];
    
      if(tmpProc->ppid == running->pid && tmpProc->status == ZOMBIE)
      {
        childPid = tmpProc->pid;
        *status = tmpProc->exitCode;

        tmpProc->status = FREE;
        putProc(&freeList, tmpProc);

        return childPid;
      }
    }

    ksleep(running);
  }
}

//gets a Proc form the freelist then sets its vaiables and clears then sets up its kstack. 
//finaly enqueueing the proc on the ready queue
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
  tmpProc->parent = running;

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
  char usrInput[MAXLEN];
  
  //get exit value in the form of an int
  printf("Enter and exit value: ");
  gets(usrInput);
    
  kexit(atoi(usrInput));
}

int do_kwait()
{
  int pid;
  int status;
  
  pid = kwait(&status);
  
  //print pid and status
  printf("pid = %d. status = %d\n", pid, status);
}

int do_kwakeup()
{
  char usrInput[MAXLEN];
  
  //get event value in the form of an int
  printf("Enter and eventValue value: ");
  gets(usrInput);

  kwakeup(atoi(usrInput));
}

int do_ksleep()
{
  char usrInput[MAXLEN];
  
  //get event value in the form of an int
  printf("Enter and exit value: ");
  gets(usrInput);

  ksleep(atoi(usrInput));
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
      tmpProc->event = 0;
      tmpProc->exitCode = 0;
      tmpProc->priority = 0;
      tmpProc->ksp = 0;
      
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
}

int scheduler()
{
  switch(running->status)
  {
    
    case READY : 
      //printf("scheduler: enqueue on readyQueue\n");
      printf("Enqueueing P%d into readyQueue\n", running->pid);
      enqueue(&readyQueue, running);
      break;

    case SLEEP : 
      //printf("scheduler: enqueue on sleepQueue\n");
      printf("Enqueueing P%d into sleepQueue\n", running->pid);
      enqueue(&sleepQueue, running);
      break;
  }

  //printf("scheduler: dequeue on readyQueue\n");
  running = dequeue(&readyQueue);
}

main()
{
  printf("\n\n********** MTX start **********\n");
  
  init();
  kfork();

  while(1)
  {
    if(readyQueue)
    {
      tswitch();
    }
    else
    {
      printf("There are no more procs to run.\n");
      printf("Good Bye.\n");
      break;
    }
  }

  return 0;
}
// main()
// {
//   char usrInput;

//   printf("\n\nMTX starts in main()\n");
  
//   init();

//   while(1){
    
//     printGlobals();  
//     printf("proc 0  running : enter a key : \n");
//     usrInput = getc();
//     kfork();
//     printf("\n");

//     //if readyQueue != empty
//     if(readyQueue)
//     {
//       tswitch();
//     }
//   }
// }



   
