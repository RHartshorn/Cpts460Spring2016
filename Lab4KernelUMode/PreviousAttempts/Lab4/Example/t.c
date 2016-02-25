#include "t.h"

int printGlobals()
{
  printf("-------------globals-------------\n");
  //printProcArray();
  printList("running", running);
  printList("freeList", freeList);
  printQueue("readyQueue", readyQueue);
  //printSleepQueue("sleepQueue", sleepQueue);
  //printStatuses();
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
  printf("u: goUmode\n");
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
      case 'f' : kfork();       break;
     // case 'f' : do_kfork();   break;          //kfork a child
     // case 's' : do_tswitch(); break;          //switch process
     // case 'q' : do_exit();    break;          //make running proc a zombie
     // case 'z' : do_ksleep();  break;          //running proc goes to sleep on an event value
     // case 'a' : do_kwakeup(); break;          //wake all procs sleeping on event
     // case 'w' : do_kwait();   break;          //wait for zombie child
     // case 'u' : goUmode();    break;          //lets the process go to user mode
     default  : printMenu();  break;
   }
  }
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
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = getProc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler()
{
    if (running->status == READY)
    {
      enqueue(&readyQueue, running);
    }
     
    running = dequeue(&readyQueue);
    color = running->pid + 0x0A;
}
            
main()
{
    printf("\n");
    printf("MTX starts in main()\n");
    while(1);
    // // initialize and create P0 as running
    // init();      
    // set_vector(80, int80h);

    // kfork("/bin/u1");     // P0 kfork() P1

    // while(1)
    // {
    //   printf("P0 running\n");
    //   while(!readyQueue);
    //   printf("P0 switch process\n");

    //   // P0 switch to run P1
    //   tswitch();         
    // }
}

