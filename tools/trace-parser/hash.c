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
#include <assert.h>

#include "util.h"
#include "list.h"
#include "hash.h"
	
/**
 * @brief Hash table.
 */
struct hash
{
	unsigned size; /**< Table size. */
	list *table;   /**< Table.      */
};

/**
 * @brief Creates a hash table.
 */
struct hash *hash_create(unsigned size)
{
	struct hash *h;
	
	/* Sanity check. */
	assert(size > 0);
	
	h = smalloc(sizeof(struct hash));
	
	/* Initialize hash. */
	h->size = size;
	h->table = scalloc(size, sizeof(list));
	for (unsigned i = 0; i < size; i++)
		h->table[i] = list_create();
	
	return (h);
}

/**
 * @brief Destroys a hash table.
 */
void hash_destroy(struct hash *h)
{
	/* Sanity check. */
	assert(h != NULL);
	
	for (unsigned i = 0; i < h->size; i++)
		list_destroy(h->table[i]);
	free(h->table);
	free(h);
}

/**
 * @brief Inserts an object in a hash table.
 */
void hash_insert(struct hash *h, void *obj)
{
	/* TODO. */
}

/**
 * @brief Retrieves an object from a hash table.
 */
void *hash_get(struct hash *h, void *key, int (*cmp)(void *, void *))
{
	/* TODO. */
}

/**
 * @brief Removes an object from the hash table.
 */
void *hash_remove(struct hash *h, void *key, int (*cmp)(void *, void *))
{
	/* TODO. */
}
