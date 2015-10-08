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

#include <mylib/util.h>
#include <mylib/object.h>
#include <mylib/vector.h>

#include "mapper.h"

/**
 * @brief Process object information.
 */
const struct objinfo process = {
	NULL, /* read()   */
	NULL, /* write()  */
	NULL, /* cmp()    */
	NULL, /* getkey() */
	NULL, /* cpy()    */
	NULL  /* free()   */
};


/**
 * @brief Creates a process.
 * 
 * @param id      Process ID.
 * @param traffic Communication traffic.
 */
struct process *process_create(int id, vector_t traffic)
{
	struct process *p;
	
	p = smalloc(sizeof(struct process));
	
	/* Initialize process. */
	p->id = id;
	p->traffic = traffic;
	
	return (p);
}

/**
 * @brief Destroys a process.
 * 
 * @param p Target process.
 */
void process_destroy(struct process *p)
{
	free(p);
}
