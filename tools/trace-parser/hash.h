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

#ifndef HASH_H_
#define HASH_H_

	#include "list.h"
	
	/**
	 * @brief Opaque pointer to a hash table.
	 */
	typedef struct hash * hash;
	
	/* Forward definitions. */
	extern hash hash_create(unsigned);
	extern void hash_destroy(hash);
	extern void hash_insert(struct hash *, void *, unsigned);
	extern void *hash_get(struct hash *, unsigned, unsigned (*)(void *));
	extern void *hash_remove(struct hash *, unsigned, unsigned (*)(void *));

#endif /* HASH_H_ */
