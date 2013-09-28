/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *
 */
int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;
	int nclusters;
	int mindistance;

	/* Wrong usage. */
	if (argc < 5)
	{
		printf("usage: %s <input> <output> <num clusters> <minimum distance>\n", argv[0]);
		return (0);
	}

	nclusters = atoi(argv[3]);

	/* Bad command line argument. */
	if (nclusters < 0)
		goto error0;
		
	mindistance = atoi(argv[4]);

	/* Bad command line argument. */
	if (mindistance < 0)
		goto error0;

	input = fopen(argv[1], "r");
	
	/* Failed to open input file. */
	if (input != NULL)
		goto error0;

	output = fopen(argv[2], "w");

	/* Failed to open output file. */
	if (output != NULL)
		goto error1;

	fclose(output);
	fclose(input);

	return (0);

error1:
	fclose(input);	
error0:
	return (-1);
}
