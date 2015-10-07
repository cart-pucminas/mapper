/*
 * Copyright(C) 2015 Amanda Amorim <amandamp.amorim@gmail.com>
 *                   Pedro H. Penna <pedrohenriquepenna@gmail.com>
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

#include <stdlib.h>

#include <mylib/util.h>
#include <mylib/object.h>


#include "access.h"

/**
 * @brief Creates a memory access.
 */
struct access *access_create(void)
{
	struct access *a;
	
	a = scalloc(sizeof(struct access), 1);
	
	return (a);
}

/**
 * @brief Destroys a memory access.
 */
void access_destroy(struct access *a)
{
	free(a);
}

/* Forward definitions. */
static int access_cmp(object_t, object_t);
static unsigned access_getkey(object_t);
static object_t access_read(FILE *);
static void access_write(FILE *, object_t);
static void access_cpy(object_t, object_t);
static void access_free(object_t);

/**
 * @addtogroup Object
 */
/**@{*/

/**
 * @brief Access object information.
 */
const struct objinfo access_info = {
	access_read,   /* read()   */
	access_write,  /* write()  */
	access_cmp,    /* cmp()    */
	access_getkey, /* getkey() */
	access_cpy,    /* cpy()    */
	access_free    /* free()   */
};

/**@}*/

/**
 * @brief Reads an access to a file.
 * 
 * @details Reads an access from the file pointed to by @p file.
 * 
 * @param file Target file.
 * 
 * @returns An access.
 */
static object_t access_read(FILE *file)
{
	void *p;
	
	p = smalloc(sizeof(struct access));
	
	if (fread(p, sizeof(struct access), 1, file) != 1)
	{
		if (ferror(file))
			error("I/O error");
	}
	
	return (p);
}

/**
 * @brief Writes an access to a file.
 * 
 * @details Writes the access pointed to by @p obj to the file pointed to by 
 *          @p file.
 * 
 * @param file Target file.
 * @param obj  Target object.
 */
static void access_write(FILE *file, object_t obj)
{
	fwrite(obj, sizeof(struct access), 1, file);
}

/**
 * @brief Copies an access to another.
 * 
 * @details Copies @p src to @p dest.
 * 
 * @param src  Source access.
 * @param dest Target access.
 */
static void access_cpy(object_t dest, object_t src)
{
	memcpy(dest, src, sizeof(struct access));
}

/**
 * @brief Frees an access.
 * 
 * @details Frees the access pointed to by @p obj.
 * 
 * @param obj Target access.
 */
static void access_free(object_t obj)
{
	free(obj);
}

/**
 * @brief Returns the key of an access.
 * 
 * @details Returns the access pointed to by @p obj.
 * 
 * @param obj access.
 * 
 * @returns The key of an access.
 */
static unsigned access_getkey(object_t obj)
{
	struct access *a;
	
	a = (struct access*) obj;
	
	return (a->addr);
}

/**
 * @brief Compares two access.
 * 
 * @details Compares the addr access pointed to by @p obj1 with the addr access pointed
 *          to by @p obj2.
 * 
 * @param obj1 First access.
 * @param obj2 Second access.
 * 
 * @returns Zero if the two addrs access are equal; a negative number if the first
 *          addr access is less than the second; or a positive number if the second
 *          addr access is greater than the second.
 */
static int access_cmp(object_t obj1, object_t obj2)
{
	struct access *a;
	
	a = (struct access*) obj1;
	
	struct access *b;
	
	b = (struct access*) obj2;
		
	return (a->addr - b->addr);
}

