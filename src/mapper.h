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

#ifndef MAPPER_H_
#define MAPPER_H_

	#include <stdbool.h>

	#include <mylib/matrix.h>
	
	/**
	 * @brief Processor's topology.
	 */
	struct processor
	{
		int height;     /**< Mesh height.              */
		int width;      /**< Mesh width.               */
		int ncores;     /**< Number of cores.          */
		int **topology; /**< Processor's topology.     */
		int *nlinks;    /**< Number of links per core. */
	};
	
	/**
	 * @brief Kmeans strategy arguments.
	 */
	struct kmeans_args
	{
		int nclusters;          /**< Number of clusters.   */
		struct processor *proc; /**< Mesh topology.        */
		int hierarchical : 1;   /**< Hierarchical mapping? */
	};
	
	/**
	 * @brief Greedy strategy arguments.
	 */
	struct greedy_args
	{
		struct processor *proc; /**< Mesh topology. */
	};
	
	/**
	 * @brief Mapping strategies.
	 */
	/**@{*/
	#define STRATEGY_KMEANS 0 /**< Kmeans strategy. */
	#define STRATEGY_GREEDY 1 /**< Greedy strategy. */
	/**@}*/

	/* Forward definitions. */
	extern int *process_map(matrix_t, int, void *);

#endif /* MAPPER_H_ */
