/*
 * Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	fprintf(stderr, "usage: trace-tune <trace> <injection\n");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Prints an error message and exits.
 */
static void error(const char *msg)
{
	fprintf(stderr, "error: %s\n");
	exit(EXIT_FAILURE);
}

/**
 * @brief Tunes a trace.
 */
int main(int argc, char **argv)
{
	FILE *filep;                  /* Trace file.              */
	float irate;                  /* Injection rate.          */
	unsigned start;               /* Start of transmission.   */
	unsigned size;                /* Message size.            */
	unsigned srcx, srcy, srcz;    /* Source coordinates.      */
	unsigned destx, desty, destz; /* Destination coordinates. */

	/* Missing arguments. */
	if (argc < 3)
		usage();
	
	sscanf(argv[2], "%f", &irate);
	if (irate < 1.0)
		irate += 1.0;
	
	filep = fopen(argv[1], "r");
	if (filep == NULL)
		error("failed to open input file");
			
	/* Read input trace file. */
	fscanf(filep, "%u %u %u %u %u %u %u %u\n",
			&start, &srcx, &srcy, &srcz, &destx, &desty, &destz, &size);
	while (!feof(filep))
	{
		start = (unsigned)round(start*irate);
		
		fprintf(stdout, "%u %u %u %u %u %u %u %u\n",
				start, srcx, srcy, srcz, destx, desty, destz, size);
		
		fscanf(filep, "%u %u %u %u %u %u %u %u\n",
				&start, &srcx, &srcy, &srcz, &destx, &desty, &destz, &size);
	}
	
	fclose(filep);
	
	return (EXIT_SUCCESS);
}
