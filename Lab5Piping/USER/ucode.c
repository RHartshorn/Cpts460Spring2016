#include "uglobals.h"
#include "uheader.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

int userCode(int userNumber)
{
  char name[64]; 
  int pid, cmd, color;

  while(1)
  {
    pid = getpid();
    color = 0x0C;
       
    printf("--------------------- u%d.c -------------------------\n", userNumber);
    printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
    show_menu();
    printf("Command ? ");
    gets(name); 
    if (name[0]==0) 
      continue;

    cmd = find_cmd(name);
    switch(cmd){
      case 0 : getpid();      break;
      case 1 : ps();          break;
      case 2 : chname();      break;
      case 3 : kfork();       break;
      case 4 : kswitch();     break;
      case 5 : wait();        break;
      case 6 : exit();        break;
      case 7 : getc();        break;
      case 8 : putc();        break;
      case 9 : fork();        break;
      case 10: uexec();       break;
      case 11: pipe();        break;
      case 12: pfd();         break;
      case 13: close();       break;
      case 14: read();        break;
      case 15: write();       break;

      default: invalid(name); break;
    }
  }
}

int geti()
{
  // return an input integer
  char *c;
  int i;
  *c = getc();
  *(c + 1) = 0;
  i = atoi(c);
}

int show_menu()
{
  int i;
  printf("***************** Menu *******************\n");
  printf("");
  for(i = 0; i < numCmd - 1; i++)
  {
    printf("%s, ", cmd[i]);
    if(!((i + 1) % 5)){printf("\n");}
  }
  printf("\n");
  printf("******************************************\n");
}

int find_cmd(char *name)
{
  int i;

  for(i = 0; i < numCmd - 1; i++)
  {
    //printf("find_cmd: cmd[i] = %s : name = %s \n", cmd[i], name);
    if(!strcmp(name, cmd[i])) return i;
  }

  return -1;
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
}    

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}
int getc()
{
  return syscall(7,0,0);
}
int putc()
{
  return syscall(8,0,0);
}

int fork()
{
  int child;

  //depending on the return value we know which will be the child and the parrent
  //sice the child returns 0.
  child = syscall(9,0,0);

  if(child)
  {
    printf("Fork(ucode): Parent pid = %d, Child pid = %d\n", getpid(), child);
  }
  else
  {
     printf("Fork(ucode): Child pid = %d, Parent pid = %d\n", getpid(), child);
  }
}

int uexec()
{
    char s[32];
    printf("input filepath : ");
    gets(s);
    return syscall(10, s, 0);
}

int invalid(char *name)
{
  printf("Invalid command : %s\n", name);
}

int pipe()
{
  printf("pipe\n");

  return syscall(11, 0, 0);

}

int pfd()
{
  printf("pfd\n");

  return syscall(12, 0, 0);
}

int close()
{
  printf("close\n");

  return syscall(13, 0, 0);
}

int read()
{
  printf("read\n");

  return syscall(14, 0, 0);
}

int write()
{
  printf("write\n");
  
  return syscall(15, 0, 0);
}
