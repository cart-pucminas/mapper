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

#include "util.h"
#include "list.h"

/**
 * @brief Creates a list node.
 */
static struct list_node *list_node_create(void *obj)
{
	struct list_node *n;
	
	n = smalloc(sizeof(struct list_node));
	
	/* Initialize the list node. */
	n->obj = obj;
	n->next = NULL;
	
	return (n);
}

/**
 * @brief Destroys a list node.
 */
static void list_node_destroy(struct list_node *n)
{
	free(n);
}

/**
 * @brief Creates a linked list.
 */
struct list *list_create(void)
{
	struct list *l;
	
	l = smalloc(sizeof(struct list));
	
	/* Initialize the list. */
	l->length = 0;
	l->head.obj = NULL;
	l->head.next = NULL;
	
	return (l);
}

/**
 * @brief Destroys a linked list.
 * 
 * @todo Remove all nodes from the list.
 */
void list_destroy(struct list *l)
{
	/* Sanity check. */
	assert(l != NULL);
	
	free(l);
}

/**
 * @brief Inserts an object in a linked list.
 */
void list_insert(struct list *l, void *obj)
{
	struct list_node *n;
	
	/* Sanity check. */
	assert(l != NULL);
	assert(obj != NULL);
	
	n = list_node_create(obj);
	
	/* Insert node in the list. */
	n->next = l->head.next;
	l->head.next = n;
	l->length++;
}

/**
 * @brief Retrieves an object from a linked list.
 */
void *list_get(struct list *l, unsigned key, unsigned (*getkey)(void *))
{
	void *obj;           /* Object to get. */
	struct list_node *w; /* Walker.        */
	
	/* Sanity check. */
	assert(l != NULL);
	assert(getkey != NULL);
	
	/* Search for object. */
	for (w = &l->head; w->next != NULL; w = w->next)
	{
		/* Found. */
		if (key == getkey(w->next->obj))
			goto found;
	}
	
	return (NULL);
	
found:

	obj = w->next->obj;	
	

	return (obj);
}

/**
 * @brief Removes an object from a linked list.
 */
void *list_remove(struct list *l,  unsigned key, unsigned (*getkey)(void *))
{
	void *obj;           /* Object to remove. */
	struct list_node *t; /* Temporary node.   */
	struct list_node *w; /* Walker.           */
	
	/* Sanity check. */
	assert(l != NULL);
	assert(getkey != NULL);
	
	/* Search for object. */
	for (w = &l->head; w->next != NULL; w = w->next)
	{
		/* Found. */
		if (key == getkey(w->next->obj))
			goto found;
	}
	
	return (NULL);
	
found:

	obj = w->next->obj;	
	
	/* Remove node from the list. */
	t = w->next;
	w->next = t->next;
	list_node_destroy(t);
	l->length--;
	
	return (obj);
}


/**
 * @brief Removes the first object from a linked list.
 */
void *list_remove_first(struct list *l)
{
	void *obj;           /* Object to remove. */
	struct list_node *t; /* Temporary node.   */
	struct list_node *h; /* Walker.           */
	
	/* Sanity check. */
	assert(l != NULL);
	assert(getkey != NULL);
	
	h = &l->head;
	
	//Lista vazia
	if(h->next == NULL)
	return(NULL);
		
	obj = h->next->obj;

	/* Remove node from the list. */
	t = h->next;
	h->next = t->next;
	list_node_destroy(t);
	l->length--;
	
	return (obj);
}
