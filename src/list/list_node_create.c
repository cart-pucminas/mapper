/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_node_create.c> - list_node_create() implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Creates a list node.
 */
struct list_node *list_node_create(void *obj)
{
	struct list_node *new_node;
	
	new_node = malloc(sizeof(struct list_node));
	
	/* Failed to allocate list node. */
	if (new_node == NULL)
		return (NULL);
	
	new_node = obj;
	new_node->next = NULL;
	
	return (new_node);
}

