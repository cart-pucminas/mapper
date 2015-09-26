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
 
#ifndef LIST_H_
#define LIST_H_

	/**
	 * @brief Linked list node.
	 */
	struct list_node
	{
		void *obj;              /**< Object.                */
		struct list_node *next; /**< Next node in the list. */
	};

	/**
	 * @brief Linked list.
	 */
	struct list
	{
		unsigned length;       /**< List length. */
		struct list_node head; /**< List head. */
	};
	
	/**
	 * @brief Opaque pointer to a linked list.
	 */
	typedef struct list *list;

	/* Forward definitions. */
	list list_create(void);
	void list_destroy(list);
	void list_insert(list, void *);
	void *list_get(list, unsigned, unsigned (*)(void *));
	void *list_remove(list, unsigned, unsigned (*)(void *));

#endif /* LIST_H_ */


