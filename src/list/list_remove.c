/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_remove.c> - list_remove() implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Removes an object from a linked list.
 */
struct list_node *list_remove(struct list *l, struct list_node *node)
{
	struct list_node *walker;
	
	/* Empty list. */
	if (l->length == 0)
		return (NULL);
	
	walker = list_head(l);
	
	if (walker == node)
	{
		assert(l->head != l->head->next);
		l->head = l->head->next;
		l->length--;
		node->next = NULL;
		return (node);
	}
	
	/* Search for node. */
	while (walker->next != NULL)
	{
		/* Found. */
		if (walker->next == node)
		{
			assert(walker != node->next);
			walker->next = node->next;
			l->length--;
			
			node->next = NULL;
			
			return (node);
		}
		
		walker = walker->next;
	}
	
	return (NULL);
}

