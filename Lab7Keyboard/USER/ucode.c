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
      case 11: createPipe();  break;
      case 12: printFD();     break;
      case 13: pipeClose();   break;
      case 14: pipeRead();    break;
      case 15: pipeWrite();   break;

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
    char str[32];

    printf("input filepath : ");
    gets(str);
    
    return syscall(10, str, 0);
}

int createPipe()
{
  syscall(11, pd, 0);
  printf("uCode: pipe, proc[%d] created pipe where fd[%d] and fd[%d] \n", getpid(), pd[0], pd[1]);

}

int printFD()
{
  return syscall(12, 0, 0);
}

int pipeClose()
{
  char str[16];
  int fd;
  
  printFD();
  
  printf("enter fd to close: ");
  gets(str);
  
  fd = atoi(str);

  return syscall(13, fd, 0);
}
 
int pipeRead()
{
  char str[16];
  char buf[1024];
  int fd;
  int bytesToRead;
  int bytesRead;

  printFD();

  printf("enter fd to read from: ");
  gets(str);
  fd = atoi(str);

  printf("enter number of bytes to read from: ");
  gets(str);
  bytesToRead = atoi(str);

  printf("reading %d bytes from fd[%d]\n", bytesToRead, fd);
  
  bytesRead = syscall(14, fd, buf, bytesToRead);

  if(bytesRead >= 0)
  {
    //make sure it ends with a end of line
    buf[bytesRead] = 0;
    printf("umode: proc[%d] read \"%s\" (%d bytes) from pipe.\n", getpid(), buf, bytesRead);
  }
  else
  {
    printf("ERROR: ucode, pipeWrite has failed \n");
  }

}

int pipeWrite()
{
  char str[16];
  char buf[1024];
  int fd;
  int bytesWritten;

  printFD();
  
  printf("enter fd to write to: ");
  gets(str);
  fd = atoi(str);

  printf("enter text to write: ");
  gets(str);
  sscanf(&str, "%s", buf);

  printf("writing \"%s\" (%d bytes) to fd%d\n", buf, strlen(buf), fd);

  bytesWritten = syscall(15, fd, buf, strlen(buf));

  if(bytesWritten >= 0)
  {
    printf("umode: proc[%d] wrote %d bytes to pipe.\n", getpid(), bytesWritten);
  }
  else
  {
    printf("ERROR: ucode, pipeWrite has failed \n");
  }

}

int invalid(char *name)
{
  printf("Invalid command : %s\n", name);
}
