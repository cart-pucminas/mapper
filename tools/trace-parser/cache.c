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

#include "util.h"
#include "list.h"
#include "hash.h"

/**
 * @brief Cache block.
 */
struct block
{
	void *obj;      /**< Underlying memory access. */
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
} cache;

/**
 * @brief Evicts an object from the cache.
 */
static void cache_evict(void)
{
	/* TODO. */
	// pega o mais antigo e coloca no swap e coloca o bloco na lista de livres
	
	unsigned old = -1; 
	unsigned old_blocks;
	
	struct block *block_tmp;
	
	for (unsigned i = 0; i < cache_size; i++){
		
		//Testar se o block é valido
		if(cache.blocks[i]->flags = 1){
			
			if(old == -1){
				//Primeiro block válido encontrado
				old = cache.blocks[i]->age;
				old_blocks = i;
			
			}else if(cache.blocks[i]->age < old){
				old = cache.blocks[i]->age;
				old_blocks = i; 
			}
	}
		
		//Remover da hash
		//Qual é a chave da hash? e a chave do objeto para comparação?
		// block_tmp = hash_remove(&table, cache.blocks[old_blocks], unsigned (*key)(void *), int (*cmp)(void *, void *))

		
		//Gravar no swap
		//Não sei manipular arquivo
		
		
		//inserir o bloco da lista de livres
		list_insert(cache.free, &cache.blocks[old_blocks]);
	
}

/**
 * @brief Inserts an object in the cache.
 */
void cache_insert(void *obj, unsigned addr)
{
	/* TODO. */
}

/**
 * @brief Updades an object in the cache.
 */
void cache_update(void *obj, unsigned addr)
{
	/* TODO. */
}

/**
 * @brief Flushes the cache to the swap file.
 */
void cache_flush(void)
{
	/* TODO. */
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
	
	/* Initialize cache. */
	cache.swp = file;
	cache.table = hash_create(cache_size >> 2);
	cache.cache_size = cache_size;
	cache.obj_size = obj_size;
	cache.blocks = scalloc(cache_size, sizeof(struct block));
	cache.free = list_create();
	
	/* Initialize list of free blocks. */
	for (unsigned i = 0; i < cache_size; i++)
		list_insert(cache.free, &cache.blocks[i]);
}
