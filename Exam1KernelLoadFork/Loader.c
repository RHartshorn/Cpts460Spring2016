#include "type.h"
#include "Loader.h"

int search(char *fileName)
{

}

//gets an inode by the filePath
int getInode(char *filePath)
{

}
//loads a given file (by path) into a segment
int load(char *fileName, u16 segment)
{

	char **kittens;
	char *strptr;

	split(fileName, '/', kittens);
	
  	//find the inode of the file name
  	//if(!foundFile){printf("Error: load, file %s not found.\n", fileName)}

	//read file header to get tsize, dsize, bsize

	//load[code|data] sections of filename to mem segmet

  //clear bss section of loaded image to 0

  return 1;
}

//UTILITY ***********

//splits a given string into a char ** array. An array 
//of char * this will allow you to go through each.
int split1(char *str, char *token, char **output)
{
	char *tmpstr;
	
	int i = 0;

char *tok = malloc(STRMAX);

	printf("%s\n", str);
	//if we were given an empty string
	if(!str)
	{
		printf("ERROR: split, string is null.\n");
		return -1;
	}

	strcpy(tmpstr, str);

	tok = strtok(tmpstr, token);

	while(tok)
	{
		printf("split tok = %s \n", tok);
		output[i] = malloc(STRMAX);
		strcpy(output[i], tok);
		tok = strtok(0, token);
		i++;
	}
	return i;
}

int split(char *str, char token, char **output)
{
	int i = 0;
	int j = 0;
	int k = 0;
	char cp;
	
	output = malloc(STRMAX);
	output[0] = malloc(STRMAX);

	//while we are not at the end of the line
	while(cp = str[k])
	{	
		if(cp == token)
		{
			output[i][j] = 0;
			i++;
			j = 0;
			printf("split: %s\n", output[i]);
			output[i] = malloc(STRMAX);
		}
		else
		{
			output[i][j] = cp;
			j++;

		}
		k++;
	}

}
