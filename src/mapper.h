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

	#include <mylib/list.h>
	#include <mylib/object.h>
	#include <mylib/vector.h>

	/*========================================================================*
	 *                          Process Interface                             *
	 *========================================================================*/

	/**
	 * @brief Process.
	 */
	struct process
	{
		int id;           /**< Process ID.            */
		vector_t traffic; /**< Communication traffic. */
	};
	
	/* Forward definitions. */
	extern struct process *process_create(int, vector_t);
	extern void process_destroy(struct process *);
	
	/* Forward definitions. */
	extern const struct objinfo process;

	/*========================================================================*
	 *                           Mapper Interface                             *
	 *========================================================================*/
	
	/* Forward definition. */
	extern list_t procs;

#endif /* MAPPER_H_ */
