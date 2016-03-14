#include "Globals.h"
#include "Header.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int printMenu()
{
  printf("-------------Menu-------------\n");
  printf("f : kfork\n" );
  printf("s : tswitch\n" );
  printf("q : exit\n" );
  printf("z : ksleep\n" );
  printf("a : kwakeup\n" );
  printf("w : kwait\n" );
  printf("p : kps\n" );
  printf("------------------------------\n");
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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
      case 'p' : kps();         break;
      default  : printMenu();   break;
    }
   }
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
//records the event value in the running proc's event then changes the running proc's status to SLEEP
//finaly it switches process
int ksleep(int eventValue)
{
  
  running->event = eventValue;
  running->status = SLEEP;
 
  //get a proc from the ready queue and make it run
  tswitch();
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
//gets a Proc form the freelist then sets its vaiables and clears then sets up its kstack. 
//finaly enqueueing the proc on the ready queue
PROC *kfork(char *filename)
{
  int i;
  int segment;
  
  PROC *tmpProc = getProc(&freeList);
  printf("kFork:\n");
  //if the proc is null
  if(!tmpProc)
  {
    printf("ERROR: kFork failed, no PROCS on freeList.\n");
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

  tmpProc->kstack[SSIZE - 1] = (int)goUmode;
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int do_kfork()
{
  kfork("/bin/u1");
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int do_tswitch()
{
  tswitch();
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int do_exit()
{
  char usrInput[MAXLEN];
  
  //get exit value in the form of an int
  printf("Enter and exit value: ");
  gets(usrInput);
    
  kexit(atoi(usrInput));
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int do_kwait()
{
  int pid;
  int status;
  
  pid = kwait(&status);
  
  //print pid and status
  printf("pid = %d. status = %d\n", pid, status);
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int do_kwakeup()
{
  char usrInput[MAXLEN];
  
  //get event value in the form of an int
  printf("Enter and eventValue value: ");
  gets(usrInput);

  kwakeup(atoi(usrInput));
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int do_ksleep()
{
  char usrInput[MAXLEN];
  
  //get event value in the form of an int
  printf("Enter and exit value: ");
  gets(usrInput);

  ksleep(atoi(usrInput));
}

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
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

//******************************************************************
//Method Name: 
//Description: 
//Comments:
//******************************************************************
int set_vector(u16 segment, u16 handler)
{
  // put_word(word, segment, offset)
  put_word(handler, 0, segment * 4);      //KPC points to handler
  put_word(0x1000,  0, segment * 4 + 2);  //KCS segment = 0x1000
}

    
