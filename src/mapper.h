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
	struct topology
	{
		unsigned height; /**< Mesh height. */
		unsigned width;  /**< Mesh width.  */
	};
	
	/**
	 * @brief Kmeans strategy arguments.
	 */
	struct kmeans_args
	{
		unsigned use_auction : 1; /**< Use auction?        */
		unsigned nclusters;       /**< Number of clusters. */
		struct topology *mesh;    /**< Mesh topology.      */
	};
	
	/**
	 * @brief Mapping strategies.
	 */
	/**@{*/
	#define STRATEGY_KMEANS 0 /**< Kmeans strategy. */
	/**@}*/

	/* Forward definitions. */
	extern int *process_map(matrix_t, unsigned, void *);
	
	/* Forward definitions. */
	extern bool verbose;

#endif /* MAPPER_H_ */
