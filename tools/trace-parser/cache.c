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

#define INVALIDO_LIMPO 0;
#define INVALIDO_SUJO 1; 
#define VALIDO_LIMPO 2;
#define VALIDO_SUJO 3;


/**
 * @brief Cache block.
 */
struct block
{
	void *obj;      /**< Underlying memory access. */
	unsigned addr;	/**< Addr.                   *
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
 * @brief Evicts an object from the cache.
 */
static void cache_evict(void)
{
	/* TODO. */
	// pega o mais antigo e coloca no swap e coloca o bloco na lista de livres
	
	int encontrar_block = 0; // tipo boolean para testar se encontrou o bloco
	unsigned i = 0;
	unsigned old; // age do mais antigo
	unsigned old_blocks; // índice do mais antigo 
	
	struct block *block_write; //bloco que será escrito
	struct block *block_read;  //bloco lido do arquivo para comparação com o bloco que será escrito
	
	//Encontrar o primeiro bloco válido para inicializar as variáveis old e old_bloks
	while (!encontrar_block && i < cache_size){
		
		//Testar se o block é valido 
		if(cache.blocks[i]->flags = VALIDO_LIMPO || cache.blocks[i]->flags = VALIDO_SUJO ){
			//Primeiro block válido encontrado
			old = cache.blocks[i]->age;
			old_blocks = i;
			encontrar_block = 1;
		}
		i++;
	}
	
	//Emcontrar o bloco mais velho
	for (unsigned j = i; j < cache_size; j++){
		
		//Testar se o block é valido e se é mais velho que o atual
		if((cache.blocks[j]->flags = VALIDO_LIMPO || cache.blocks[j]->flags = VALIDO_SUJO )
			&& cache.blocks[j]->age < old){
				old = cache.blocks[j]->age;
				old_blocks = j; 
		}
	}
		
		//Remover da hash
		/* A função não deveria receber as funções key (retorna chave da hash) e
		 * cmp (usado para comparar o bloco procurado com os blocos das listas da hash) para serem passadas
		 * para a função hash_remove ao invés de ter addr no block
		 */
		
		// block_write = hash_remove(&table, cache.blocks[old_blocks], unsigned (*key)(void *), int (*cmp)(void *, void *))

		
		//Gravar no swap
		
		// Posicionar o ponteiro do arquivo na posição que o bloco será escrito
		fseek(cache.swp, (block_write->offset * cache.obj_size), SEEK_SET); 
		fwrite( block_write, cache.obj_size, 1 , cache.swp);
		
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
	cache.age = 0;
	
	/* Initialize list of free blocks. */
	for (unsigned i = 0; i < cache_size; i++)
		list_insert(cache.free, &cache.blocks[i]);
}
