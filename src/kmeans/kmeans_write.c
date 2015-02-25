/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_write.c> - kmeans_write() implementation.
 *
 * FIXME: support other topologies.
 */

#include <assert.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include "kmeans.h"

/*
 * Table.
 */
struct table
{
	int height;
	int width;
	int *elements;
};
	
/*
 * Returns the element [i, j] in a table.
 */
#define TABLE(t, i, j) ((t)->elements[(t)->width*(i) + (j)])

/*
 * Creates a table.
 */
static struct table *table_create(int height, int width)
{
	struct table *t;
	
	t = malloc(sizeof(struct table));
	
	/* Failed to allocate table. */
	if (t == NULL)
		goto error0;

	t->height = height;
	t->width = width;
	
	t->elements = calloc(height*width, sizeof(int));
	
	/* Failed to allocate elements. */
	if (t->elements == NULL)
		goto error1;

	return (t);

error1:
	free(t);
error0:
	return (NULL);
}

/*
 * Destroys a table.
 */
static void table_destroy(struct table *t)
{
	free(t->elements);
	free(t);
}

/*
 * Region count.
 */
static int count;

/*
 * Internal implementation of table_split().
 */
static void _table_split(struct table *t, int i0, int j0, int height, int width, int size)
{
	int i, j;
	
	
	/* Stop condition reached. */
	if (width*height <= size)
	{
		/* Enumerate region. */
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
				TABLE(t, i0 + i, j0 + j) = count;
		}
		
		count++;
		
		return;
	}
	
	/* Split vertically. */
	if (width > height)
	{
		_table_split(t, i0, j0, height, width/2, size);
		_table_split(t, i0, j0 + width/2, height, width/2, size);
	}
	
	/* Split horizontally. */
	else
	{
		_table_split(t, i0, j0, height/2, width, size);
		_table_split(t, i0 + height/2, j0, height/2, width, size);
	}
}

/*
 * Splits a table recursively.
 */
static void table_split(struct table *t, int i0, int j0, int height, int width, int size)
{
	count = 0;
	_table_split(t, i0, j0, height, width, size);
}

/*
 * Writes kmeans() result in a file.
 */
void kmeans_write(FILE *output)
{
	int c;
	unsigned i, j;
	struct list_node *n;
	struct table *map;
	
	map = table_create(noc.height, noc.width);
	assert(map != NULL);

	table_split(map, 0, 0, noc.height, noc.width, nprocs/nclusters);	
	
	/* Map processes. */
	for (i = 0; i < noc.height; i++)
	{
		for (j = 0; j < noc.width; j++)
		{
			c = TABLE(map, i, j);
			
			n = list_remove_first(clusters[c].procs);
			
			fprintf(output, "%d %d\n", PROCESS(n)->id, i*noc.width + j);
			
			list_insert(procs, n);
		}
	}

	table_destroy(map);
}
