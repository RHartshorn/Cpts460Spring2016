#include "Globals.h"
#include "Header.h"
#include "ext2.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 2/5/16
//Last Edited: 3/29/16
//Project: Computer Science 460: Exam 1
//******************************************************************

//takes a string and returns an array of substrings 
//created by splitting at the delimiter. handesl empty
//strings and removes back to back delimiters etc.
int strSplit(char* string, char **strArray, char *delim)
{
	char *strptr;
	char *str;

	int stringCount = 0;
	int charCount = 0;

	strArray[stringCount] = malloc(MAXLENPATH);

	strcpy(str, string);
	strptr = str;

	//go untill end of string
	while(1)
	{
		//if we are at a delimiter
		if(*strptr == *delim || *strptr == '\0')
		{
			//if the substring is empty
			if(charCount > 0)
			{
				strArray[stringCount][charCount] = '\0';
				charCount = 0;
				stringCount++;
				strArray[stringCount] = malloc(MAXLENPATH);
			}
			if(*strptr == '\0')
			{
				return stringCount;
			}
		}
		else
		{
			strArray[stringCount][charCount] = *strptr;
			charCount++;
		}
		strptr++;
	}
}

//converts and assci to an int
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

//******************************************************************
//Method Name: get_block
//Description: retreives the given blcok from the file.
//Comments: get_block is a helper function provided by KCW to ease
//			obtaining a block from the file disk into memory.
//******************************************************************
u16 get_block(u16 blk, char buf[])
{	
	//Convert blk into CYL, HEAD, SECTOR format by mailman to suit disc geometry.
	//		CYL			HEAD 					SECTOR
	diskr((blk / 18), ((2 * blk) % 36) / 18, ((2 * blk) % 36) % 18, buf);	
}