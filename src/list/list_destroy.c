/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_destroy.c> - list_destroy() implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Destroys a linked list.
 */
void list_destroy(struct list *l)
{
	/* Remove all objects from the list. */
	while (l->size > 0)
		list_node_destroy(list_remove(l));
	
	free(l);
}
