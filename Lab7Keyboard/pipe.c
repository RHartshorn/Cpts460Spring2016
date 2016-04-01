#include "Globals.h"
#include "Header.h"
#include "ext2.h"

//******************************************************************
//Author: Robin Hartshorn
//Student ID: 10906124
//Date Created: 3/10/16
//Last Edited: 3/10/16
//Project: Computer Science 460: Lab 5
//******************************************************************

//******************************************************************
//Method Name: kpipe
//Description: takes a pipe discriptor from umode. 
//Comments: assumes there is a free pipe struct and two free ofts
//******************************************************************
int kpipe(int pd[2])
{
	int i;
	PIPE *pptr;
	OFT *oftptr1, *oftptr2;

	printf("kpipe: \n");
	//find a pipe that is not in use
	for(i = 0; i < NPIPE; i++)
	{
		//if the pipe is free
		if(pipe[i].status == PIPE_FREE)
		{
			printf("  pipe[%d] = PIPE_FREE\n", i);
			//set the free pipe to the pipe pointer and set the pipe ptr to busy
			pptr = &pipe[i];
			pptr->status = PIPE_IN_USE;
			
			//set the head tail and data in the pipe
			pptr->head = 0;
			pptr->tail = 0;
			pptr->data = 0;

			pptr->room = PSIZE;

			pptr->nreader = 1;
			pptr->nwriter = 1;
			
			break;
		}
	}

	//find a oft that will be the READ oft
	for(i = 0; i < NOFT; i++)
	{
		//if we have found a free oft
		if(!oft[i].refCount)
		{
			printf("  oft[%d] = READ_PIPE\n", i);
			//set the mode to read and the oft's pipe_ptr to the pipe we are creating
			oftptr1 = &oft[i];
			oftptr1->refCount = 1;
			oftptr1->mode = READ_PIPE;
			oftptr1->pipe_ptr = pptr;
			break;
		}	
	}

	//find a oft that will be the WRITE oft
	for(i = 0; i < NOFT; i++)
	{
		//if we have found a free oft
		if(!oft[i].refCount)
		{
			printf("  oft[%d] = WRITE_PIPE\n", i);
			//set the mode to write and the oft's pipe_ptr to the pipe we are creating
			oftptr2 = &oft[i];
			oftptr2->refCount = 1;
			oftptr2->mode = WRITE_PIPE;
			oftptr2->pipe_ptr = pptr;
			break;
		}	
	}

	//go through the running proc's fd[] and find a pair of free procs
	for(i = 0; i < NFD; i+=2)
	{
		//if we have found a pair of fd in the array that are free
		if(!running->fd[i] && !running->fd[i + 1])
		{
			printf("  running proc's fd[%d] = READ_PIPE\n", i);
			printf("          proc's fd[%d] = WRITE_PIPE\n", i + 1);
			//set the Read oft to even fd and then pass it to uspace
			running->fd[i] = oftptr1;
			put_word(i, running->uss, pd[0]);
			
			//set the Write oft to even fd and then pass it to uspace
			running->fd[i + 1] = oftptr2;
			put_word(i + 1, running->uss, pd[1]);
			break;
		}
	}

	return 0;

}

//******************************************************************
//Method Name: kclose
//Description: closing the file descriptor at a given location.
//Comments:
//******************************************************************
int kclose(int fdLocation)
{

	OFT *fdptr;
	PIPE *pipeptr;

	printf("kclose: \n");
	printf("  closing running->fd[%d]\n", fdLocation);

	fdptr = running->fd[fdLocation];

	//check to make sure we have a valid fd we are closing
	if(!fdptr)
	{
		printf("ERROR: kclose, fd[%d] is already closed.\n", fdLocation);
		return -1;
	}
	
	//set the fd at the given location to null
	running->fd[fdLocation] = 0;

	//if we have a writing pipe
	if(fdptr->mode == WRITE_PIPE)
	{
		pipeptr = fdptr->pipe_ptr;

		//decroment the number of writers in the pipe and the refcount
		pipeptr->nwriter--;
		fdptr->refCount--;

		//if we are the last one writing to the pipe
		if(fdptr->refCount < 1)	
		{
			//if there are no readers
			if(pipeptr->nreader < 1)
			{
				//set the pipe free
				pipeptr->status = PIPE_FREE;
				return 1;
			}
		}
		//wake up all blocked readers
		kwakeup(&pipeptr->data);
		return 1;
	}
	else if(fdptr->mode == READ_PIPE)
	{
		pipeptr = fdptr->pipe_ptr;

		//decroment the number of readers in the pipe and the refcount
		pipeptr->nreader--;
		fdptr->refCount--;

		//if we are the last one writing to the pipe
		if(fdptr->refCount < 1)	
		{
			//if there are no readers
			if(pipeptr->nwriter < 1)
			{
				//set the pipe free
				pipeptr->status = PIPE_FREE;
				return 1;
			}
		}
		//wake up all blocked writers
		kwakeup(&pipeptr->room);
		return 1;
	}
	else
	{
		printf("ERROR: kclose, fd[%d] is neather a READ_PIPE nor a WRITE_PIPE.\n", fdLocation);
		return -1;
	}
}

//******************************************************************
//Method Name: kread
//Description: 
//Comments: returns -1 for failure or the number of bytes read
//******************************************************************
int kread(int fdLocation, char *buf, int nbytes)
{
	int bytesRead = 0;
	char c;
	OFT *fdptr;
	PIPE *pipeptr;


	printf("kread: \n");
	printf("  reading %d bytes from running->fd[%d]\n", nbytes, fdLocation);

	if(fdLocation > NFD || fdLocation < 0)
	{
		printf("ERROR: kread, fd[%d] is an invalid file discriptor.\n", fdLocation);
		return -1;
	}
	if(!running->fd[fdLocation])
	{
		printf("ERROR: kread, fd[%d] is null.\n", fdLocation);
		return -1;
	}

	fdptr = running->fd[fdLocation];
	pipeptr = fdptr->pipe_ptr;

	if(fdptr->mode != READ_PIPE)
	{
		printf("ERROR: kread, fd[%d] is not a READ_PIPE.\n", fdLocation);
		return -1;
	}

	while(bytesRead < nbytes)
	{
		//while there is stuff left to read and we have not finished reading
		while(pipeptr->data > 0 && bytesRead < nbytes)
		{
			c = pipeptr->buf[pipeptr->tail];
			put_byte(c, running->uss, buf);

			//incroment the tail and loop if we are past the end
			pipeptr->tail++;
			pipeptr->tail %= PSIZE;

			pipeptr->data--;
			pipeptr->room++;
			
			bytesRead++;
			buf++;
		}
		if(bytesRead > 0)
		{
			kwakeup(&pipeptr->room);
			return bytesRead;
		}
		//if we have run out of things to read from the pipe.
		//if the pipe still has a writer
		if(pipeptr->nwriter)
		{
			//wake up procs waiting for room and sleep on data
			kwakeup(&pipeptr->room);
			sleep(&pipeptr->data);

			continue;
		}

		return 0;
	}



}

//******************************************************************
//Method Name: kwrite
//Description: 
//Comments:
//******************************************************************
int kwrite(int fdLocation, char *buf, int nbytes)
{
	int bytesWriten = 0;
	char c;
	OFT *fdptr;
	PIPE *pipeptr;


	printf("kwrite: \n");
	printf("  writing %d bytes to running->fd[%d]\n", nbytes, fdLocation);

	if(fdLocation > NFD || fdLocation < 0)
	{
		printf("ERROR: kwrite, fd[%d] is an invalid file discriptor.\n", fdLocation);
		return -1;
	}
	if(!running->fd[fdLocation])
	{
		printf("ERROR: kwrite, fd[%d] is null.\n", fdLocation);
		return -1;
	}

	fdptr = running->fd[fdLocation];
	pipeptr = fdptr->pipe_ptr;

	if(fdptr->mode != WRITE_PIPE)
	{
		printf("ERROR: kwrite, fd[%d] is not a WRITE_PIPE.\n", fdLocation);
		return -1;
	}

	while(bytesWriten < nbytes)
	{
		//if no one is reading
		if(pipeptr->nreader < 1)
		{
			printf("ERROR: kwrite, proc[%d]: the pipe is broken.", running->pid);
			kexit(0x0D);
		}
		//while there is space in the buf and we have stuff to write
		while(pipeptr->room && bytesWriten < nbytes)
		{
			pipeptr->buf[pipeptr->head] = get_byte(running->uss, buf);

			pipeptr->head++;
			pipeptr->head %= PSIZE;

			pipeptr->data++;
			pipeptr->room--;

			bytesWriten++;
			buf++;
		}

		kwakeup(&pipeptr->data);

		//if we are finished writing
		if(bytesWriten >= nbytes)
		{
			return bytesWriten;
		}

		//we still have more to write sleep untill there is room
		ksleep(&pipeptr->room);
	}
		
	return bytesWriten;
}

