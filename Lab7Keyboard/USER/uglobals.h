#ifndef UGLOBALS_H
#define UGLOBALS_H

#define LEN 64

char *cmd[] = {"getpid", "ps", "chname", "kfork", "switch", 
			  "wait", "exit", "getc", "putc", "fork", 
			  "exec", "createPipe", "printFD", "pipeClose", "pipeRead", 
			  "pipeWrite", 0 }; 

int numCmd = sizeof(cmd)/2;

int pd[2];

#endif