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
	while (l->length > 0)
		list_node_destroy(list_remove_first(l));
	
	free(l);
}
