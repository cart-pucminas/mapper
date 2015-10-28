/*
 * Copyright(C) 2011-2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Mapper.
 *
 * Mapper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MyLib. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include <mylib/util.h>

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: map2nas <nas file> <map file>\n");
	printf("Brief: instruments a NAS trace file with a map file.\n");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Finds the core ID for a given thread.
 */
static int find_coreid(FILE *mapfile, int threadid)
{
	int coreid;
	int threadid2;
	
	fseek(mapfile, 0, SEEK_SET);
	
	/* Find coreid for src. */
	while (fscanf(mapfile, "%d %d", &threadid2, &coreid) != EOF)
	{
			if (threadid2 == threadid)
				return (coreid);
	}
	
	return (-1);
}

/**
 * @brief Instruments a NAS trace file with a map file.
 */
static void map2nas(FILE *nasfile, FILE *mapfile)
{
	int size;    /* Size of the message.   */
	int dest;    /* Destination thread.    */
	int src;     /* Source thread.         */
	float start; /* Start of transmission. */
	
	/* Instrument NAS trace file. */
	while (fscanf(nasfile, "%f %d %d %d", &start, &src, &dest, &size) != EOF)
	{
		int coreid[2];
		
		if ((coreid[0] = find_coreid(mapfile, src)) < 0)
			error("source thread not found");
		if ((coreid[1] = find_coreid(mapfile, dest)) < 0)
			error("destination thread not fond");
		
		printf("%f %d %d %d", start, coreid[0], coreid[1], size);
	}
}

/**
 * @brief Instruments a NAS trace file with a map file.
 */
int main(int argc, char **argv)
{
	FILE *mapfile; /* Thread map file. */
	FILE *nasfile; /* NAS file.        */
	
	/* Wrong usage. */
	if (argc != 3)
		usage();
	
	/* Open map file. */
	if ((mapfile = fopen(argv[1], "r")) == NULL)
		error("cannot open NAS trace file");
	
	/* Open NAS trace file. */
	if ((nasfile = fopen(argv[2], "r")) == NULL)
		error("cannot open NAS trace file");
	
	map2nas(nasfile, mapfile);
	
	/* House keeping. */
	fclose(nasfile);
	fclose(mapfile);
	
	return (EXIT_SUCCESS);
}

