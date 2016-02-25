#include "type.h"
#include "t.h"
#include "io.h"
#include "Queue.h"

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
      case 'f' : do_kfork();    break;         //kfork a child
      case 's' : do_tswitch();  break;         //switch process
      case 'q' : do_exit();     break;         //make running proc a zombie
      case 'z' : do_ksleep();   break;         //running proc goes to sleep on an event value
      case 'a' : do_kwakeup();  break;         //wake all procs sleeping on event
      case 'w' : do_kwait();    break;         //wait for zombie child
      case 'u' : goUmode();     break;
      default  : printMenu();   break;
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
PROC *kfork(char *filename)
{
  int i;
  int segment;
  
  PROC *tmpProc = getProc(&freeList);
 
  //if the proc is null
  if(!tmpProc)
  {
    printf("ERROR: kFork failed, empty PROC list.\n");
    return 0;
  }

  segment = (tmpProc->pid + 1) * 0x1000;
  tmpProc->status = READY;
  tmpProc->priority = 1;
  tmpProc->ppid = running->pid;
  tmpProc->parent = running;

  //setting all of the items on the kstack to 0
  for(i = 1; i < 10; i++)
  {
    tmpProc->kstack[SSIZE - 9] = 0;
  }

  tmpProc->kstack[SSIZE - 1] = (int)body;
  tmpProc->ksp  = &tmpProc->kstack[SSIZE - 9];
  
  if(filename)
  {
    tmpProc->uss = segment;

    load(filename, segment);

    tmpProc->usp = -24; //the low end of the stack

    //setting all of the registers on the ustack to 0
    for(i = 1; i < 13; i++)
    {
      put_word(0, segment, i * (-2));
    } 

    //set uDS user data segment
    put_word(segment, segment, 12 * (-2));
    //set uES to segment 
    put_word(segment, segment, 11 * (-2));
    //set uCS user code segment uCS = user code segment
    put_word(segment, segment, 2 * (-2));
    //set flag I bit-1 for allow iterrupts
    put_word(segment, 0x2000, 1 * (-2));
  }

  enqueue(&readyQueue, tmpProc);

  return tmpProc;
}

int do_kfork()
{
  kfork("/bin/u1");
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
    PROC *p; int i;
    color = 0x0C;
    printf("init ....");
    
    // initialize all procs
    for (i=0; i<NPROC; i++)
    {   
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->next = &proc[i+1];
    }

    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepQueue = 0;

    /**** create P0 as running ******/
    p = getProc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    //nproc = 1;
    printf("running Proc %s", running->name);
    printf("done\n");
}

int scheduler()
{

  if (running->status == RUNNING)
  {
    running->status = READY;
    enqueue(&readyQueue, running);
  }
  
  running = dequeue(&readyQueue);
  running->status = RUNNING;
  color = running->pid + 0x0A;
}

main()
{
    printf("\n");
    printf("MTX starts in main()\n");
    
    // initialize and create P0 as running
    init();      

    set_vector(80, int80h);

    kfork("/bin/u1");     // P0 kfork() P1

    while(1)
    {
      printf("P0 running\n");
      while(!readyQueue);
      printf("P0 switch process\n");

      // P0 switch to run P1
      tswitch();
      
    }
}

int set_vector(u16 segment, u16 handler)
{

  // put_word(word, segment, offset)
  put_word(handler, 0, segment * 4);      //KPC points to handler
  put_word(0x1000,  0, segment * 4 + 2);  //KCS segment = 0x1000
}

    
