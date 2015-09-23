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

#include <stdint.h>

#include "util.h"
#include "hash.h"

/**
 * @brief Memory access.
 */
struct access
{
	uint64_t addr;  /** Address.            */
	int access[12]; /**< Access per thread. */
};

/**
 * @brief Cache block.
 */
struct block
{
	struct access *a; /**< Underlying memory access. */
	unsigned flags;   /**< Flags.                    */
	unsigned age;     /**< Age for evicting.         */
};

/**
 * @brief Cache.
 */
struct cache
{
	hash *table; /**< Memory access table. */
	list free;   /**< List of free blocks. */
}

/**
 * @brief Creates a memory access.
 */
static struct access *access_create(void)
{
	/* TODO. */
}

/**
 * @brief Destroys a memory access.
 */
static void access_destroy(struct access *a)
{
	/* TODO. */
}

/**
 * @brief Reads a memory access from the swap file.
 */
static struct access *access_read(uint64_t addr)
{
	/* TODO. */	
}

/**
 * @brief Writes a memory access to the swap file.
 */
static void access_write(struct access *a)
{
	/* TODO. */
}

/**
 * @brief Initializes the cache.
 */
void cache_init(void)
{
	/* TODO. */
}
