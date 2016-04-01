#include "Globals.h"
#include "Header.h"
#include "ext2.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

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


//STATUS ***********
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

//INODE ***********

//******************************************************************
//Method Name: PrintGroup
//Description: Prints out relevant GD info
//Comments:
//******************************************************************
void PrintGroup(GD * gp)
{
  printf("--------------------GROUP DESCRIPTOR DATA-------------------\n");
  printf("bg_block_bitmap = %d\n", gp->bg_block_bitmap);
  printf("bg_inode_bitmap = %d\n", gp->bg_inode_bitmap);
  printf("bg_inode_table = %d\n", gp->bg_inode_table);
  printf("bg_free_blocks_count = %d\n", gp->bg_free_blocks_count);
  printf("bg_free_inodes_count = %d\n", gp->bg_free_inodes_count);
  printf("bg_used_dirs_count = %d\n", gp->bg_used_dirs_count);
  printf("bg_inode_table = %d\n", gp->bg_inode_table);
  printf("------------------------------------------------------------\n\n");
  getc();
}

//******************************************************************
//Method Name: PrintInodeData
//Description: Output relevant Inode data
//Comments: This function split from LoadInode
//******************************************************************
void PrintInodeData(INODE * ip)
{
  printf("--------------------------- INODE --------------------------\n"); 
  printf("Mode = %4x \n", ip->i_mode);
  printf("User ID = %d\n", ip->i_uid);
  printf("Group ID = %d\n", ip->i_gid);
  printf("File Size = %d(bytes)\n", ip->i_size);
  //printf("time=%s\n", ctime((time_t *)&ip->i_ctime));
  printf("Link Count = %d\n", ip->i_links_count);
  printf("Blocks Count = %d\n", ip->i_blocks);
  printf("i_block[0] = %d\n", ip->i_block[0]);
  printf("------------------------------------------------------------\n\n");
  getc();
}


//IO: DATASTRUCTURES **********
void PrintHeader(HEADER *head)
{
  printf("-------------------------- HEADER -------------------------\n");
  printf("head->ID_space = %d\n", head->ID_space);
  printf("head->magic_number = %d\n", head->magic_number);
  printf("head->tsize = %d\n", head->tsize);
  printf("head->dsize = %d\n", head->dsize);
  printf("head->bsize = %d\n", head->bsize);
  printf("head->zero = %d\n", head->zero);
  printf("head->total_size = %l\n", head->total_size);
  printf("head->symbolTable_size = %d\n", head->symbolTable_size);
  printf("------------------------------------------------------------\n\n");
  getc();
}


//******************************************************************
//Method Name: 
//Description: Print file discriptor
//Comments: 
//******************************************************************
int pfd(OFT *fd)
{
  int i;
  OFT *tmpFd;
  
  printf("FD\t\tType\t\tContents\t\t\n");
  printf("-------------------------------------------------\n");
  for(i = 0; i < NFD; i++)
  {
    if(running->fd[i])
    {
      printf("%d\t\t%s\t\t\n", i, pipeModeArray[running->fd[i]->mode]);
    }
  }
  printf("-------------------------------------------------\n");
}

//******************************************************************
//Method Name: 
//Description: Prints the contents of the given pipe
//Comments: 
//******************************************************************
int printPipeContents(PIPE *pipeptr)
{
  
}
