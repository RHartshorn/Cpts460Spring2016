#include "Kernel.h"
 
//kfork from lab 3
PROC *kfork(char *filename)
{
	// kfork as BEFORE: pid = new PROC's pid
	//int segment = (pid + 1) * 0x1000;		// new PROC's segment = (pid+1)*0x1000;

	// if we have been given a file
	if(filename)
	{
		//load filename to childs segment as its Umode image
		
		//setup childs ustack for it to return to umode 
		//to execute the the loaded image

	}

	//return child proc ptr
}

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



