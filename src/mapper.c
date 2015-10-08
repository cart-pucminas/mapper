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

#include <mylib/list.h>
#include <mylib/matrix.h>
#include <mylib/vector.h>

#include "mapper.h"

/**
 * @brief Processes.
 */
list_t procs;

/**
 * @brief Maps process.
 */
void process_map(unsigned nprocs, matrix_t communication)
{
	procs = list_create(&process);
	
/* Create processes. */
	for (unsigned i = 0; i < nprocs; i++)
	{
		vector_t traffic;
		struct process *proc;
		
		traffic = vector_create(nprocs*nprocs);
		
		for (unsigned j = 0; j < nprocs; j++)
		{
			vector_set(traffic, i*nprocs + j, matrix_get(communication, i, j));
			vector_set(traffic, j*nprocs + i, matrix_get(communication, j, i));
		}
		
		proc = process_create(i, traffic);
		
		list_insert(procs, proc);
	}
}
