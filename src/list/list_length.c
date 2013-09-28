/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_length.c> - list_length() implementation.
 */

#include <list.h>

/*
 * Returns the length of a list.
 */
int list_length(struct list *l)
{
	return (l->length);
}


