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
// //loads a given file (by path) into a segment
// int load1(char *filename, u16 segment)
// {
	
// 	GD *gp;
// 	INODE *ip;
// 	HEADER *head;

// 	u16 ino, bbino;
// 	int i;

// 	printf("loading from file = %s\n", filename);
// 	ino = getInode(filename);
// 	printf("Got Inode num = %d from filename = %s\n", ino, filename);
// 	getc();

// 	if(!ino)
// 	{
// 		printf("ERROR: load, file %s not fount.\n", filename);
// 		return -1;
// 	}

// 	//get GD from filename
// 	get_block(2, LoadBuf);
// 	gp = (GD *)LoadBuf; 
// 	bbino = (u16)gp->bg_inode_table;

// 	PrintGroup(gp);

// 	//get iblock from previously gotten ino
// 	//get_block((ino - 1) / 8 + bbino, LoadBuf);
// 	get_block(bbino, LoadBuf);
// 	//ip = (INODE *)LoadBuf + (ino - 1) % 8;
// 	ip = (INODE *)LoadBuf + 1;

// 	PrintInodeData(ip);
// 	PrintHeader(head);	
	
// 	//set the segment to the place we want to write
// 	setes(segment);

// 	printf("Loading . . .\n");

// 	//load direct blocks
// 	for(i = 0; i < 12; i++)
// 	{

// 	}

// 	//load indirect blocks


// 	//set the segment back to kernel segment
// 	setes(0x1000);


// 	//read file header to get tsize, dsize, bsize

// 	//load[code|data] sections of filename to mem segmet

// 	//clear bss section of loaded image to 0

//   return 1;
// }

//******************************************************************
//Method Name: Search
//Description: Find the name within a given directory, return inode
//Comments: This function is only called by GetInode
//******************************************************************
// int search(INODE *ip, char *filename)
// {
// 	int remove = 0;
// 	int i = 0; 
// 	char *cp = LoadBuf;
// 	DIR *dp = (DIR *)LoadBuf;
// 	INODE *iptmp;

// 	printf("Search: \n");
// 	getc();
// 	//search all of the blocks of the inode
// 	for(i = 0; i < 12; i++)
// 	{

// 		printf("looking in block %d\n", i);

// 		printf("size of = %d\n", sizeof(ip->i_block[i]));
// 		getc();

// 		get_block((u16)ip->i_block[i], LoadBuf);
		
// 		while(cp < LoadBuf + BLKSIZE)
// 		{
// 			strncpy(LoadBuf2, dp->name, dp->name_len);
// 			LoadBuf2[dp->name_len] = 0;
			//NEED TO SAVE BIT then restore After comparison
// 			printf("  found string = %s\n", LoadBuf2);
// 			if(!strcmp(filename, LoadBuf2))
// 			{
// 				printf("  found\n");
// 				return dp->inode;
// 			}

// 			cp += dp->rec_len;
// 			dp = (DIR *)cp;
// 		}
// 	}
// 	printf("  not found\n");
// 	return 0;
// }

//gets an inode by the filePath
// int getInode(char *filepath)
// {
// 	char *strptr;
// 	char *filename;
// 	char *pathpeace;

// 	int i = 0; 

// 	int ino = 2;
// 	INODE *ip;

// 	GD *GDptr;
// 	printf("getInode\n");
	
// 	printf("  loading group\n");
// 	//get the group descriptor block
// 	//load it into buf then have GDptr point at it
// 	LoadGroup(&GDptr, 2);

// 	printf("  loading inode\n");
// 	//load the root node into inoptr
// 	LoadInode(ino, GDptr->bg_inode_table, &ip);

// 	//printf("  copy filepath %s to filename\n", filepath);
// 	strcpy(filename, filepath);
	
// 	pathpeace = strtok(filename, "/");
	
// 	while(pathpeace)
// 	{
// 		printf("pathpeace = %s\n", pathpeace);
// 		ino = search(ip, pathpeace) - 1;

// 		if(ino > 0)
// 		{
// 			printf("load from ino %d\n", ino);
// 			LoadInode(ino, GDptr->bg_inode_table, &ip);
// 			pathpeace = strtok(0,"/");
// 		}
// 	}
// 	return ino;
// }

// int getInode(char *filepath)
// {
// 	u16 iblock, ino;
// 	int i;
	
// 	char *namefrag;
// 	char filepathcopy[MAXLENPATH];

// 	INODE *ip;
// 	GD *gp;

// 	printf("Search:\n");
// 	printf("  Given filepath = %s\n", filepath);
// 	//get the group discriptor so we can get inode table start block
// 	get_block(2,LoadBuf);
// 	gp = (GD *)LoadBuf;

// 	//cast the start block into a u16 and use getblock
// 	iblock = (u16)gp->bg_inode_table;
// 	get_block(iblock, LoadBuf);

// 	//points at the root inode
// 	ip = (INODE*)LoadBuf + 1;

// 	strcpy(filepathcopy, filepath);
// 	namefrag = strtok(filepathcopy, "/");

// 	//use search
// 	while(namefrag)
// 	{
// 		printf("  namefrag = %s\n", namefrag);
// 		ino = search(ip, namefrag) - 1;

// 		if(ino < 0){printf("ERROR: getInode illegal value for ino.\n");return 0;}

// 		get_block(iblock +(ino / 8), LoadBuf);
// 		ip = (INODE *)LoadBuf + (ino % 8);
	
// 		namefrag = strtok(0,"/");
// 	}

// 	return ino;

// }	

//loads a binary executable file into mem for execution. 
// int load(char *filepath, u16 segment)
// {
// 	char filepathcopy[MAXLENPATH];
// 	char *pathpeace;

// 	u16 ino, iblock, word, seg, rem, i, j;
// 	u32 *indblockptr;

// 	GD * gp;
// 	INODE *ip;
// 	HEADER *headerptr;

// 	printf("Load:\n");
// 	printf("  Loading from file = %s\n", filepath);

// 	//get the group discriptor so we can get inode table start block and then
// 	//cast the start block into a u16. iblock is the starting block for the inodes 
// 	get_block(2, LoadBuf);
// 	gp = (GD *)LoadBuf;
// 	iblock = (u16)gp->bg_inode_table;

// 	PrintGroup(gp);

// 	//cast the start block into a u16 and use getblock
// 	get_block(iblock, LoadBuf);
// 	ip = (INODE *)LoadBuf + 1;

// 	PrintInodeData(ip);

// 	//copy filepath name then cut the path to peaces and start searching the inode
// 	strcpy(filepathcopy, filepath);
// 	pathpeace = strtok(filepathcopy, "/");

// 	//go untill we have no more peaces
// 	while(pathpeace)
// 	{
// 		printf("  pathpeace = %s\n", pathpeace);
// 		//searches the dir entrys for the given string
// 		//inodes have multiple dirs which contan the len of the 
// 		//entry and names and where they start
// 		ino = search(ip, pathpeace) - 1;

// 		//if we return a negative inumber while searching we have failed
// 		if(ino < 0)
// 		{
// 			printf("ERROR: load, file not found check path.\n");
// 			return 0;
// 		}

// 		//load up the next block to check and get the next path peace
// 		//ino / number of blocks in segment. set ip = the block with that ino
// 		get_block(iblock + (ino / 8), LoadBuf);
// 		ip = (INODE *)LoadBuf + (ino % 8);
// 		pathpeace = strtok(0,"/");
// 	}

// 	//read the file header into the buffer and store its values
// 	getblk((u16)ip->i_block[0], LoadBuf2);
// 	headerptr = (HEADER *)LoadBuf2;

// 	PrintHeader(headerptr);

// 	PrintInodeData(ip);
	
// 	//set the segment to the place we want to write. 
// 	//we still are exicuting in kernel space but are writing to the 
// 	//new location.
// 	//by setting es = (tmpProc->pid + 1) * 0x1000;
// 	setes(segment);

// 	//load the direct blocks
// 	printf("  Loading Direct Blocks. . . \n");
// 	printf("  ");

// 	//read all 12 blocks and stop if we encounter an empty one
// 	for(i = 0; i < 12 && ip->i_block[i]; i++)
// 	{
// 		//since we are not giving it a buf to write into it will write where it is
// 		//this will be the place es is pointing at. Think of buffer as on offset 
// 		//incroment es by 1k
// 		get_block((u16)ip->i_block[i], 0);
// 		inces();
// 		printf("*");
// 	}
// 	printf("\n");

// 	//check to see if there are indirect blocks
// 	if(ip->i_block[12])
// 	{
// 		printf("  Loading Indirect Blocks . . . \n");
// 		printf("  ");
		
// 		//read indirect blocks into buf and setup the indirect block ptr
// 		get_block((u16)ip->i_block[12], LoadBuf3);
// 		indblockptr = (u32 *)LoadBuf3;

// 		//while we have indirect blocks to wirte
// 		while(*indblockptr)
// 		{
// 			//see above + we are moving the indirect block pointer over
// 			get_block((u16 *)indblockptr, 0);
// 			inces();
// 			indblockptr++;
// 			printf(".");
// 		}
// 		printf("\n");
// 		printf("  Finished Loading Indirect Blocks\n");
// 	}
// 	else
// 	{
// 		printf("  No Indirect Blocks\n");
// 	}
	
// 	//move what we have writen to cover header while 32 bytes
// 	//i <= (code section size + initialized data section)
// 	//32 * 1024
// 	for(i = 0; i <= headerptr->tsize + headerptr->dsize; i += 2)
// 	{
// 		word = get_word(segment + 2, i);
// 		put_word(word, segment, i);
// 	}

// 	//clear bss section of loaded image set to 0
// 	seg = segment + (headerptr->tsize + headerptr->dsize) / 16;
// 	rem = headerptr->tsize % 16;

// 	printf("  Clearing BSS to 0\n");

// 	for(i = 0; i < rem; i++)
// 	{
// 		put_byte(0, seg, i);
// 	}

// 	for(j = 0; j < headerptr->bsize; j++)
// 	{
// 		put_byte(0, seg, j + i);
// 	}

// 	//move back to kernel space by setting es back to 0x1000
// 	setes(0x1000);

// 	printf("  Load finished\n");

// 	return 1;

// }