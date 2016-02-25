
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/
#include "type.h"

#define AX 8
#define PA 13

extern PROC *running, *freelist, *readyqueue;

/****************** syscall handler in C ***************************/
int kcinth()
{
  u16 segment, offset;

  int a,b,c,d;
  int retValue;

  segment = running->uss;
  offset = running->usp;
  
  //get the syscall perameters from the ustack
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

      case 99: kkexit(b);            break;

      default: printf("invalid syscall # : %d\n", a); 
  }

  //put the return value in uax
  put_word(retValue, segment, offset + 2 * AX);
}

//============= WRITE C CODE FOR syscall functions ======================

int kgetpid()
{
    //WRITE YOUR C code
}

int kps()
{
    //WRITE C code to print PROC information
}

int kchname(char *name)
{
    //WRITE C CODE to change running's name string;
}

int kkfork()
{
  //use you kfork() in kernel;
  //return child pid or -1 to Umode!!!
}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{

  //use YOUR kwait() in LAB3;
  //return values to Umode!!!
}

int kkexit(int value)
{
    //use your kexit() in LAB3
    //do NOT let P1 die
}

