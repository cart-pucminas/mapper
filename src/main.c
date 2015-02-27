/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <main.c> - main() function of the application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <kmeans.h>

/**
 * @brief Mesh topology.
 */
struct
{
	unsigned height;
	unsigned width;
} noc;

/**
 * @brief Number of clusters.
 */
unsigned nclusters;

/**
 * @brief Minimum distance.
 */
unsigned mindistance;

/**
 * @brief Prints an error message and exits.
 */
static void error(const char *msg)
{
	fprintf(stderr, "error: %s\n", msg);
	exit(EXIT_FAILURE);
}

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: mapper <input> <topology> <nclusters> <mindistance>\n");
	printf("Brief: maps processes on a manycore processor");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Reads command line arguments.
 */
static void readargs(int argc, char **argv)
{
	/* Missing arguments. */
	if (argc < 5)
	{
		printf("missing arguments\n");
		usage();
	}

	sscanf(argv[2], "%u %*c %u", &noc.height, &noc.width);
	if ((noc.height == 0) || (noc.width == 0))
		error("invalid manycore processor size");
	
	sscanf(argv[3], "%u", &nclusters);
	if ((nclusters == 0))
		error("invalid number of clusters");
	
	sscanf(argv[4], "%u", &mindistance);
}

/*
 * Mapps processes in a NoC
 */
int main(int argc, char **argv)
{
	FILE *input;
	
	readargs(argc, argv);

	/* Open input file. */
	input = fopen(argv[1], "r");
	if (input == NULL)
		error("failed to open input file");

	kmeans(nclusters, mindistance, input);

	fclose(input);

	return (0);
}
