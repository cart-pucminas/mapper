/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_remove.c> - Linked list library implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Removes an object from a linked list.
 */
struct list_node *list_remove(struct list *l)
{
	struct list_node *node;
	
	/* The list is empty. */
	if (l->size == 0)
		return (NULL);
	
	node = l->head;
	
	/* Remove object from the list. */
	l->head = l->head->next;
	l->size--;
	
	return (node);
}
