#ifndef UTIL_H
#define UTIL_H

#include "type.h"

int atoi(char *str);
char *gets(char str[]);

char *ascciTable = "0123456789ABCDEF";

int set_vector(u16 segment, u16 handler);
// int put_word(u16 word, u16 segment, u16 offset);
// int put_byte(u8 byte, u16 segment, u16 offset);

// int get_word(u16 segment, u16 offset); //gets a word
// u8 get_byte(u16 segment, u16 offset);

#endif