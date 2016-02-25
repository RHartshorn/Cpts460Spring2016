#include "util.h"

int atoi(char *str)
{
  int retVal = 0;
  int sign = 1;
  char *strPtr = str;

  if(*strPtr == '-')
  {
    sign = -1;
    strPtr++;
  }
  //go untill the end of the string
  while(*strPtr != 0)
  {
    retVal *= 10;
    retVal += (int)*strPtr - '0';
    
    strPtr++;
  }

  return retVal * sign;
}

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

//from book the "segment" is a "vector"
int set_vector(u16 segment, u16 handler)
{

  // put_word(word, segment, offset)
  put_word(handler, 0, segment * 4);      //KPC points to handler
  put_word(0x1000,  0, segment * 4 + 2);  //KCS segment = 0x1000
}


// int put_word(u16 word, u16 segment, u16 offset)
// {
//   setds(segment);
//   *(u16 *)offset = word;
//   setds(MTXSEG);
// }
// int put_byte(u8 byte, u16 segment, u16 offset)
// {
//   setds(segment);
//   *(u8 *)offset = byte;
//   setds(MTXSEG);
// }

// int get_word(u16 segment, u16 offset)
// {
//   u16 word;
//   setds(segment);
//   word = *(u16 *)offset;
//   setds(MTXSEG);
//   return word;
// }
// u8 get_byte(u16 segment, u16 offset)
// {
//   u8 byte;
//   setds(segment);
//   byte = *(u8 *)offset;
//   setds(MTXSEG);
//   return byte;
// }