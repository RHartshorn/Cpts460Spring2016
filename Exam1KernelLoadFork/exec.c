#include "Globals.h"
#include "Header.h"

int exec(char *filepath)
{

	int i = 0, len = 0;
	char filename[MAXPATHNAMELEN];
	char *cp = filename;
	u16 segment = running->uss;
	char c = 'a';

	printf("Exec:\n");

	//get the filepath from mem
	while(c)
  	{
		//name is used as the offset here think of its address value
		c = get_byte(running->uss, filepath + i);
		filename[i] = c;
		i++;
  	}
  	printf("  filename = %s\n", filename);

  	//load the filename into segment
	if(!load(filename, segment))
	{
		printf("  ERROR: exec, load failed.\n");
		return -1;
	}

	//setting up umode stack to
	for(i = 1; i <= 12; i++)
	{
		//writing low to high must be 2 from the end
		//to accomidate the first word.
		put_word(0, segment, -2 * i);
	}

	//lowest end of the stack incromenting by 12 * -2
	running->usp = -24;

	//set uDS to segment
	put_word(segment, segment, -2 * 12);
	//set uES to segment
	put_word(segment, segment, -2 * 11);
	//set uCS to segment
	put_word(segment, segment, -2 * 2);
	//set umode flag = 0x2000
	put_word(0x2000, segment, -2 * 1);

	return 1;

}