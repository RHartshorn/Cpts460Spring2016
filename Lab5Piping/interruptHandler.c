#include "Globals.h"
#include "Header.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

#define AX 8
#define PA 13

/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
  int segment, offset;

  int a,b,c,d;
  int retValue;

  segment = running->uss;
  offset = running->usp;
  
  //get the syscall perameters from the ustack
  //a what is being called this is given to us by
  //the ucode syscall. the others are peramiters. 
  a = get_word(segment, offset + 2 * (PA));
  b = get_word(segment, offset + 2 * (PA + 1));
  c = get_word(segment, offset + 2 * (PA + 2));
  d = get_word(segment, offset + 2 * (PA + 3));

  //route the syscall to kernal functions using a
  switch(a)
  {
      case 0 : retValue = kgetpid();        break;
      case 1 : retValue = kps();            break;
      case 2 : retValue = kchname(b);       break;
      case 3 : retValue = kkfork();         break;
      case 4 : retValue = ktswitch();       break;
      case 5 : retValue = kkwait(b);        break;
      case 6 : retValue = kkexit(b);        break;
      case 7 : retValue = getc();           break;
      case 8 : retValue = putc(b);          break;
      case 9 : retValue = fork();           break;
      case 10: retValue = exec(b);          break;
      case 11: retValue = kpipe();          break;
      case 12: retValue = pfd();            break;
      case 13: retValue = kclose();         break;
      case 14: retValue = kread();          break;
      case 15: retValue = kwrite();         break; 

      case 99: kkexit(b);                   break;

      default: printf("invalid syscall # : %d\n", a); 
  }

  //put the return value in uAX
  put_word(retValue, segment, offset + 2 * AX);
}

//============= WRITE C CODE FOR syscall functions ======================

//return the running procs pid
int kgetpid()
{
  return running->pid;
}

int kps()
{
  int i;
  PROC *tmpProc;

  printf("Name\t\tStatus\t\tPid\t\tppid\n");
  printf("-------------------------------------------------\n");
  for(i = 0; i < NPROC; i++)
  {
    tmpProc = &proc[i];
    printf("%s\t\t%s\t\t%d\t\t%d\n", tmpProc->name, statusArray[tmpProc->status], tmpProc->pid, tmpProc->ppid);
  }
  printf("-------------------------------------------------\n");
}

int kchname(char *name)
{
  char c = 'a';
  int i = 0;

  while(c)
  {
    //name is used as the offset here think of its address value
    c = get_byte(running->uss, name + i);
    running->name[i] = c;
    i++;
  }
}

// int kexec(char *filepath)
// {
//   char c;
//   int i = 0;

//   while(c)
//   {
//     c = get_byte(running->uss, filepath + i);
//     FilePathArray[i] = c;
//     i++;
//   }
//   printf("Kexecs: filepatharray =  %s\n", filepatharray);
//   exec(filepatharray); 
// }

int kkfork()
{
  //use you kfork() in kernel;
  //return child pid or -1 to Umode!!!
  return kfork("/bin/u1");

}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{
  int retValue;
  //use YOUR kwait() in LAB3;
  //return values to Umode!!!
  retValue = kwait(status);
  put_word(*status, running->uss, status);
  return retValue;

}

int kkexit(int value)
{
  //use your kexit() in LAB3
  //do NOT let P1 die
  if(running->pid == 1)
  {
    printf("Error: kkexit, you cant exit form P1 in user mode.\n");
    return -1;
  }

  return kexit(value);
}

