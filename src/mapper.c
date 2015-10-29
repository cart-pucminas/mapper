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

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include <mylib/matrix.h>
#include <mylib/vector.h>
#include <mylib/util.h>

#include "mapper.h"

/* Forward definitions. */
extern int *map_kmeans(matrix_t, void *);
extern int *map_greedy(matrix_t, void *);

/**
 * @brief Number of mapping strategies.
 */
#define NR_STRATEGIES 2

/**
 * @brief Mapping strategy.
 */
typedef int *(*strategy)(matrix_t, void *);

/**
 * @brief Mapping strategies.
 */
static strategy strategies[NR_STRATEGIES] =  {
	map_kmeans,
	map_greedy
};

/**
 * @brief Maps process.
 */
int *process_map(matrix_t communication, int strategy, void *args)
{
	int *map;
	
	/* Sanity check. */
	assert(communication != NULL);
	assert(matrix_height(communication) == matrix_width(communication));
	assert(strategy < NR_STRATEGIES);
	assert(args != NULL);
	
	map = strategies[strategy](communication, args);
	
	return (map);
}
