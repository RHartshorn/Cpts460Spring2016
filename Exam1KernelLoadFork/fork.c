#include "Globals.h"
#include "Header.h"

//forkes a child process with an identical Umode image as the running pids
//the parent returns the child's pid, the child returns 0.
int fork()
{

  u16 segment, word;
  
  PROC *tmpProc;
 
  printf("Fork:\n");

  tmpProc = kfork(0);

  //if the proc is null
  if(!tmpProc)
  {
    printf("ERROR: kFork failed, no PROCS on freeList.\n");
    return 0;
  }

  printf("  new procs name = %s\n", tmpProc->name);

  segment = (tmpProc->pid + 1) * 0x1000;
  
  copy_image(segment);

  //usp will be the same as the parants usp
  tmpProc->uss = segment;
  tmpProc->usp = running->usp;
  
  //goes to umode insted of kmode
  tmpProc->kstack[SSIZE - 1] = (int)goUmode;
  tmpProc->ksp  = &tmpProc->kstack[SSIZE - 9];
  
  //set uDS to segment
  put_word(segment, segment, tmpProc->usp);
  //set uES to segment 
  put_word(segment, segment, tmpProc->usp + 2);
  //set uAX to 0 returns 0
  put_word(0, segment, tmpProc->usp + 2 * 8);
  //set uCS to segment
  put_word(segment, segment, tmpProc->usp + 2 * 10);

  printf("  fork is done \n", tmpProc->name);
  
  //return the pid
  return tmpProc->pid;
}

//DIFFERS FROM BOOK
int copy_image(u16 segment)
{
  u16 word, i;
  //copy from the parrents Proc's uss to the
  //tmpProc's segment/uss. copying 32k words
  for(i = 0; i < 32 * 1024; i++)
  {
    word = get_word(running->uss, 2 * i);
    put_word(word, segment, 2 * i);
  }
  
  return 1;
}
