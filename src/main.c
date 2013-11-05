/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <main.c> - main() function of the application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <kmeans.h>

/*
 * NoC mesh topology.
 */
struct
{
	int height;
	int width;
} noc;

/*
 * Mapps processes in a NoC
 */
int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;
	int nclusters;
	int mindistance;

	/* Wrong usage. */
	if (argc < 6)
	{
		printf("Usage: %s <input> <output> <mesh topology> <num clusters> <minimum distance>\n", argv[0]);
		return (0);
	}

	sscanf(argv[3], "%d%*c%d", &noc.height, &noc.width);
	
	/* Bad command line argument. */
	if ((noc.height & 2) != 0)
		goto error0;
	if ((noc.width & 2) != 0)
		goto error0;
	if ((noc.height <= 0) || (noc.width <= 0))
		goto error0;

	nclusters = atoi(argv[4]);

	/* Bad command line argument. */
	if (nclusters <= 0)
		goto error0;
	
	mindistance = atoi(argv[5]);
	
	/* Bad command line argument. */
	if (mindistance <= 0)
		goto error0;	
	
	input = fopen(argv[1], "r");
	
	/* Failed to open input file. */
	if (input == NULL)
		goto error0;

	output = fopen(argv[2], "w");

	/* Failed to open output file. */
	if (output == NULL)
		goto error1;
	
	kmeans(nclusters, mindistance, input, output);

	fclose(output);
	fclose(input);

	return (0);

error1:
	fclose(input);	
error0:
	return (-1);
}
