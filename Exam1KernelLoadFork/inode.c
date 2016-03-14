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

int LoadInode(int ino, int InodeBeginBlock, INODE **ip)
{
	int blocknumber, inodenumber;
	//char buf[MAXLEN];

	printf("LoadInode\n");

	blocknumber = (ino - 1) / 8 + InodeBeginBlock;
	inodenumber = (ino - 1) % 8;

	printf("  getblock bnum = %d inum = %d\n",blocknumber, inodenumber);
	get_block((u16)blocknumber, LoadBuf);

	*ip = (INODE *)LoadBuf + inodenumber;

	PrintInodeData(*ip);

	return 1;
}

//******************************************************************
//Method Name: LoadGroup
//Description: Loads a Group Descriptor into given GD pointer
//Comments: After loading the Group Descriptor pointer the function
//			returns the inode starting block number.
//******************************************************************
int LoadGroup(GD **gp, int GDBlockNumber)
{
	// read GROUP block
	int iblock;
	printf("LoadGroup:\n");
	get_block((u16)GDBlockNumber, LoadBuf);
	*gp = (GD *)LoadBuf;

	iblock = (*gp)->bg_inode_table;   // get inode start block#

	PrintGroup(*gp);

	return iblock;
}
