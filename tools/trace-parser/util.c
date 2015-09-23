/*
 * Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *                            Amanda Amorim <amandamp.amorim@gmail.com>
 * 
 * This file is part of Mapper.
 * 
 * Mapper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mapper. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Prints an error message and exits.
 */
void error(const char *msg)
{
	fprintf(stderr, "error: %s\n", msg);
	exit(EXIT_FAILURE);
}

/**
 * @brief Safe malloc().
 */
void *smalloc(size_t size)
{
	void *p;
	
	if ((p = malloc(size)) == NULL)
		error("cannot malloc()");
	
	return (p);
}

/**
 * @brief Safe calloc().
 */
void *scalloc(size_t nmemb, size_t size)
{
	void *p;
	
	if ((p = calloc(nmemb, size)) == NULL)
		error("cannot malloc()");
	
	return (p);
}
