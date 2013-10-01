/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_remove_first.c> - list_remove_first() implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Removes the first object from a linked list.
 */
struct list_node *list_remove_first(struct list *l)
{
	struct list_node *node;
	
	/* The list is empty. */
	if (l->length == 0)
		return (NULL);
	
	node = l->head;
	
	assert(l->head != l->head->next);
	
	/* Remove object from the list. */
	l->head = l->head->next;
	l->length--;
	
	node->next = NULL;
	
	return (node);
}

