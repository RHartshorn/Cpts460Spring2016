#include "io.h"

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
