/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_next.c> - list_next() implementation.
 */

#include <list.h>

/*
 * Returns the next node of a list.
 */
struct list_node *list_next(struct list_node *node)
{
	return (node->next);
}

