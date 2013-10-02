/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_insert()> - list_insert() implementation.
 */

#include <list.h>

/*
 * Inserts a list node in a linked list.
 */
int list_insert(struct list *l, struct list_node *node)
{
	/* Insert node. */
	node->next = l->head;
	l->head = node;
	l->length++;
	
	return (0);
}
