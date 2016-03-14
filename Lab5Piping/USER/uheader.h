#ifndef UHEADER_H
#define UHEADER_H

//ucode ***********
int userCode(int userNumber);

//get ***********
int geti();

//system calls ***********
int getpid();
int ps();
int chname();
int kfork();
int kswitch();
int wait();
int exit();
int _exit(int exitValue);
int getc();
int putc();
int fork();
int uexec();
int invalid(char *name);

//system calls:pipe ***********
int pipe();
int pfd();
int close();
int read();
int write();

#endif