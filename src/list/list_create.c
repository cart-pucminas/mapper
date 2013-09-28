/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list/list_create.c> - list_create() implementation.
 */

#include <list.h>
#include <stdlib.h>

/*
 * Creates a liked list.
 */
struct list *list_create()
{
	struct list *l;
	
	l = malloc(sizeof(struct list));
	
	/* Failed to allocate list. */
	if (l == NULL)
		return (NULL);
	
	/* Initialize the list. */
	l->length = 0;
	l->head = NULL;
	
	return (l);
}
