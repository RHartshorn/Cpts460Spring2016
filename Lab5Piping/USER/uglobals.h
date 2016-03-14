#ifndef UGLOBALS_H
#define UGLOBALS_H

#define LEN 64

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", "getc", "putc", "fork", "exec", "pipe", "pfd", "close", "read", "write", 0};
int numCmd = sizeof(cmd)/2;

#endif