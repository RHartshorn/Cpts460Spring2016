//Robin Hartshorn
//cpts 460
//1-25-16
//io.c

//max string lenth
#define MAXLEN 128

//define types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

char *CharTable = "0123456789ABCDEF";

//functions 

//recursivly prints a number
int rpu(u16 num, u16 base)
{
	
	char c;
	
	if(num)
	{
		c = CharTable[num % base];
		rpu(num / base, base);
		putc(c);
	}
}
//recursivly prints a number
int rpl(u32 num, u32 base)
{
	
	char c;
	
	if(num)
	{
		c = CharTable[num % base];
		rpu(num / base, base);
		putc(c);
	}
}

//prints an unsigned int of size 16, in base 10
int printu(u16 num)
{
	(num == 0) ? putc('0') : rpu(num, (u16)10);	
}  

//prints an unsigned int of size 16, in hex
int printx(u16 num)
{
	(num == 0) ? putc('0') : rpu(num, (u16)16);	
}  

//prints a long unsigned int of size 32, in base 10
int printl(u32 num)
{
	(num == 0) ? putc('0') : rpl(num, (u32)10);	
}

//takes a string and prints using putchar
int prints(char *str)
{
	//goes untill it hits the terminating '/0' char
	while(*str)
	{
		putc(*str++);
	}
}

//this is basicly printf from the lib function
int printf(char *fmt, ...)
{
    
	char *cp = fmt;
	u16 *ip = (u16 *) &fmt + 1;
	u32 *up;

	u16 flag = 0;
	
	while(*cp)
	{

		if(flag == 0)
		{
			if(*cp == '%')
			{
				flag = 1;
			}
			else if(*cp == '\n')
			{
				putc('\n');
				putc('\r');
			}
			else
			{
				putc(*cp);
			}
		}
		else
		{
			//will only be hit after a '%' is found
			switch(*cp)
			{

				case 'd' : printu(*ip); ip++; break;
				case 's' : prints(*ip); ip++; break;
				case 'x' : printx(*ip); ip++; break;
				//ip incromented to accomidate the 32 bit size
				case 'l' : printl(*ip); ip+=2; break;
				default: putc('%'); putc(*cp); break;	
			}

			flag = 0;
			ip++;
		}

		cp++;

	}
}

//gets input in the form of a string form the terminal
char *gets (char str[])
{
	char c, *t = str;
	int len = 0;

	while(((c = getc()) != '\r') && (len < (MAXLEN - 1)))
	{
		*t++ = c;
		putc(c);
		len++;
	}

	*t = 0;
	return str;
}