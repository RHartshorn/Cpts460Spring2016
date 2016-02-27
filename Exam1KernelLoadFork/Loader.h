#ifndef LOADER_H
#define LOADER_H

#define STRMAX 1024

int load(char *fileName, u16 segment);


//UTILITY***********
int split(const char *str, char *token, char **output);

#endif