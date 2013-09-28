/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_head.c> - list_head() implementation.
 */

#include <list.h>

/*
 * Returns the first node of a list.
 */
struct list_node *list_head(struct list *l)
{
	return (l->head);
}
