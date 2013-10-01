/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <list.h> - Linked list library.
 */

#ifndef LIST_H_
#define LIST_

	#include <assert.h>

	/*
	 * List node.
	 */
	struct list_node
	{
		void *obj;              /* Stored object.         */
		struct list_node *next; /* Next node in the list. */
	};

	/*
	 * Linked list.
	 */
	struct list
	{
		unsigned length;        /* List length. */
		struct list_node *head; /* List head.   */
	};
	
	/*
	 * Creates a linked list.
	 */
	extern struct list *list_create();
	
	/*
	 * Destroys a linked list.
	 */
	extern void list_destroy(struct list *l);
	
	/*
	 * Creates a list node.
	 */
	extern struct list_node *list_node_create(void *obj);
	
	/*
	 * Destroys a list node.
	 */
	extern void list_node_destroy(struct list_node *node);
	
	/*
	 * Inserts a list node in a linked list.
	 */
	int list_insert(struct list *l, struct list_node *node);
	
	/*
	 * Removes an object from a linked list.
	 */
	extern struct list_node *list_remove(struct list *l, struct list_node *node);
	
	/*
	 * Removes the first object from a linked list.
	 */
	extern struct list_node *list_remove_first(struct list *l);
	
	/*
	 * Returns the first node of a list.
	 */
	extern struct list_node *list_head(struct list *l);
	
	/*
	 * Returns the next node of a list.
	 */
	extern struct list_node *list_next(struct list_node *node);
	
	/*
	 * Returns the length of a list.
	 */
	int list_length(struct list *l);
	
	/*
	 * Returns the object stored in a list node.
	 */
	#define OBJECT(node) \
		((node)->obj)

#endif /* LIST_H_ */
