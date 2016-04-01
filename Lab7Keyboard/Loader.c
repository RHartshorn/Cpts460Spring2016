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

//loads a binary executable file into mem for execution. 
int load(char *filepath, u16 segment)
{
	char filepathcopy[MAXLENPATH];
	char *pathpeace;

	u16 ino, iblock, word, seg, rem, i, j;
	u32 *indblockptr;

	GD * gp;
	INODE *ip;
	HEADER *headerptr;

	printf("Load:\n");
	printf("  Loading from file = %s\n", filepath);

	//get the group discriptor so we can get inode table start block and then
	//cast the start block into a u16. iblock is the starting block for the inodes 
	get_block(2, LoadBuf);
	gp = (GD *)LoadBuf;
	iblock = (u16)gp->bg_inode_table;

	PrintGroup(gp);

	//cast the start block into a u16 and use getblock
	get_block(iblock, LoadBuf);
	ip = (INODE *)LoadBuf + 1;

	PrintInodeData(ip);

	//copy filepath name then cut the path to peaces and start searching the inode
	strcpy(filepathcopy, filepath);
	pathpeace = strtok(filepathcopy, "/");

	//go untill we have no more peaces
	while(pathpeace)
	{
		printf("  pathpeace = %s\n", pathpeace);
		//searches the dir entrys for the given string
		//inodes have multiple dirs which contan the len of the 
		//entry and names and where they start
		ino = search(ip, pathpeace) - 1;

		//if we return a negative inumber while searching we have failed
		if(ino < 0)
		{
			printf("ERROR: load, file not found check path.\n");
			return 0;
		}

		//load up the next block to check and get the next path peace
		//ino / number of blocks in segment. set ip = the block with that ino
		get_block(iblock + (ino / 8), LoadBuf);
		ip = (INODE *)LoadBuf + (ino % 8);
		pathpeace = strtok(0,"/");
	}

	//read the file header into the buffer and store its values
	get_block((u16)ip->i_block[0], LoadBuf2);
	headerptr = (HEADER *)LoadBuf2;

	PrintHeader(headerptr);

	PrintInodeData(ip);

	//set the segment to the place we want to write. 
	//we still are exicuting in kernel space but are writing to the 
	//new location.
	//by setting es = (tmpProc->pid + 1) * 0x1000;
	setes(segment);

	//load the direct blocks
	printf("  Loading Direct Blocks. . . \n");
	printf("  ");

	//read all 12 blocks and stop if we encounter an empty one
	for(i = 0; i < 12 && ip->i_block[i]; i++)
	{
		//since we are not giving it a buf to write into it will write where it is
		//this will be the place es is pointing at. Think of buffer as on offset 
		//incroment es by 1k
		get_block((u16)ip->i_block[i], 0);
		inces();
		printf("*");
	}
	printf("\n");

	//check to see if there are indirect blocks
	if(ip->i_block[12])
	{
		printf("  Loading Indirect Blocks . . . \n");
		printf("  ");
		
		//read indirect blocks into buf and setup the indirect block ptr
		get_block((u16)ip->i_block[12], LoadBuf3);
		indblockptr = (u32 *)LoadBuf3;

		//while we have indirect blocks to wirte
		while(*indblockptr)
		{
			//see above + we are moving the indirect block pointer over
			get_block((u16 *)indblockptr, 0);
			inces();
			indblockptr++;
			printf(".");
		}
		printf("\n");
		printf("  Finished Loading Indirect Blocks\n");
	}
	else
	{
		printf("  No Indirect Blocks\n");
	}
	
	//move what we have writen to cover header while 32 bytes
	//i <= (code section size + initialized data section)
	//32 * 1024
	for(i = 0; i <= headerptr->tsize + headerptr->dsize; i += 2)
	{
		word = get_word(segment + 2, i);
		put_word(word, segment, i);
	}
	
	//move back to kernel space by setting es back to 0x1000
	setes(0x1000);

	printf("  Load finished\n");

	return 1;

}
