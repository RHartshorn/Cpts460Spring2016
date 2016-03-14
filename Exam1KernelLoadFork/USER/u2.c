#include "ucode.c"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

int color;
main()
{ 
  char name[64]; int pid, cmd;

  while(1)
  {
    pid = getpid();
    color = 0x0C;
       
    printf("--------------------- u2.c -------------------------\n");
    printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
    show_menu();
    printf("Command ? ");
    gets(name); 
    if (name[0]==0) 
        continue;

    cmd = find_cmd(name);
    switch(cmd){
           case 0 : getpid();   break;
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : kfork();    break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;
           case 6 : exit();     break;
           case 7 : getc();     break;
           case 8 : putc();     break;
	         case 9 : fork();     break;
           case 10: uexec();     break;
           
           default: invalid(name); break;
    }
  }
}




