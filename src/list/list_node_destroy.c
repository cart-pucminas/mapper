/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_node_destroy.c> - list_node_destroy() implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Destroys a list node.
 */
void list_node_destroy(struct list_node *node)
{
	free(node);
}


