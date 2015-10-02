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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "util.h"
#include "list.h"
#include "hash.h"

/**
 * @brief Block flags.
 */
/**@{*/
#define BLOCK_VALID (1 << 0)
#define BLOCK_DIRTY (1 << 1)
/**@}*/


/**
 * @brief Cache block.
 */
struct block
{
	void *obj;      /**< Underlying memory access. */
	unsigned addr;	/**< Addr.                     */
	long offset;    /**< Offset.                   */
	unsigned flags; /**< Flags.                    */
	unsigned age;   /**< Age for evicting.         */
};

/**
 * @brief Cache.
 */
struct
{
	FILE *swp;            /**< Swap file.            */
	hash table;           /**< Cache table.          */
	unsigned cache_size;  /**< Cache size.           */
	size_t obj_size;      /**< Object size.          */
	struct block *blocks; /**< Cache blocks.         */
	list free;            /**< List of free blocks.  */
	unsigned age; 		  /**< Cache age.            */
} cache;

/**
 * @brief Retrieves the key from a cache block.
 */
static unsigned getkey(void *obj)
{
	return (((struct block *)obj)->addr);
}


/**
 * @brief Evicts an object from the cache.
 */
static void cache_evict(void)  
{
	unsigned oldest = UINT_MAX;
	
	/* Search for oldest block. */
	for (unsigned i = 0; i < cache.cache_size; i++)
	{
		/* Skip invalid blocks. */
		if (!(cache.blocks[i].flags & BLOCK_VALID))
			continue;
			
		/* First valid block. */
		if (oldest == UINT_MAX)
		{
			oldest = i;
			continue;
		}
		
		/* Oldest block. */
		if (cache.blocks[i].age < cache.blocks[oldest].age)
			oldest = i;
	}
	
	 /* All blocks are invalid. */
	 if (oldest == UINT_MAX)
		return;
		
	/* Remove block from the hash table. */
	hash_remove(cache.table, cache.blocks[oldest].addr, getkey);

	/* Write block to swap file. */
	fseek(cache.swp, (cache.blocks[oldest].offset*cache.obj_size), SEEK_SET); 
	fwrite(cache.blocks[oldest].obj, cache.obj_size, 1, cache.swp);
	
	/* Insert block in the list of free blocks. */
	cache.blocks[oldest].flags &= ~BLOCK_VALID;
	list_insert(cache.free, &cache.blocks[oldest]);
}

/**
 * @brief Inserts an object in the cache.
 */
void cache_insert(void *obj, unsigned addr)
{
	struct block *b;
	
	do
	{
		b = list_remove_first(cache.free);

		/* Evict oldest block from the cache. */
		if (b == NULL)
		{
			cache_evict();
			continue;
		}
	} while (b == NULL);
	
	/* Initialize block. */
	b->obj = obj;
	b->addr = addr;
	b->age = cache.age;
	b->flags = BLOCK_VALID;
	b->offset = -1;
 	
	/* Insert block in the hash table. */
	hash_insert(cache.table, b, addr);
}

/**
 * @brief Updades an object in the cache.
 */
void cache_update(void *obj, unsigned addr)
{
	
	/* TODO. */
	struct block *b;
	
	//Procurar addr na cache
	
	b = hash_get(cache.table, addr, getkey);

	if (b == NULL){
		error("failed to update cache");
	}else{
		
		b->age = cache.age;
		b->flags = BLOCK_VALID;
		
		memcpy(b->obj, obj, cache.obj_size);
	}
	
}

/**
 * @brief Flushes the cache to the swap file.
 */
void cache_flush(void)
{
	/* Flush all valid dirty blocks in the swap file. */
	for (unsigned i = 0; i < cache.cache_size; i++)
	{
		/* Skip invalid and clean blocks. */
		if (!(cache.blocks[i].flags & BLOCK_VALID))
			continue;
		if (!(cache.blocks[i].flags & BLOCK_DIRTY))
			continue;
			
		/* Find offset in swap file. */
		if (cache.blocks[i].offset < 0)
		{
			void *tmp;
			
			tmp = smalloc(cache.obj_size);
			
			fseek(cache.swp, 0, SEEK_SET);
			do
			{
				/* Read object. */
				fread(tmp, cache.obj_size, 1, cache.swp);
				if (ferror(cache.swp))
					error("failed to flush cache");
				if (feof(cache.swp) || ferror(cache.swp))
					break;
				
				/* Not this object. */
				if (memcmp(tmp, cache.blocks[i].obj, cache.obj_size))
					continue;
				
				/* Save offset. */
				cache.blocks[i].offset = ftell(cache.swp) - cache.obj_size;
			} while (!feof(cache.swp));
		}
		
		
	}
}

/**
 * @brief Initializes the cache.
 */
void cache_init(FILE *file, size_t obj_size, unsigned cache_size)
{
	/* Sanity check. */
	assert(file != NULL);
	assert(obj_size > 0);
	assert(cache_size > 0);
	assert(cache_size < UINT_MAX);
	
	/* Initialize cache. */
	cache.swp = file;
	cache.table = hash_create(cache_size >> 2);
	cache.cache_size = cache_size;
	cache.obj_size = obj_size;
	cache.blocks = scalloc(cache_size, sizeof(struct block));
	cache.free = list_create();
	cache.age = 0;
	
	/* Initialize list of free blocks. */
	for (unsigned i = 0; i < cache_size; i++)
		list_insert(cache.free, &cache.blocks[i]);
}
