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

	#include <mylib/matrix.h>
	
	/**
	 * @brief Kmeans strategy arguments.
	 */
	struct kmeans_args
	{
		unsigned use_auction : 1; /**< Use auction?        */
		unsigned nclusters;       /**< Number of clusters. */
	};
	
	/**
	 * @brief Mapping strategies.
	 */
	/**@{*/
	#define STRATEGY_KMEANS 0 /**< Kmeans strategy. */
	/**@}*/

	extern int *process_map(matrix_t, unsigned, void *);

#endif /* MAPPER_H_ */
