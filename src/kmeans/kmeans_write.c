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
void kmeans_write(FILE *input)
{
	int c;
	unsigned start;
	unsigned i, j;
	unsigned size;
	unsigned xsrc, ysrc, src;
	unsigned xdest, ydest, dest;
	struct list_node *n;
	struct table *maptab;
	unsigned *map;
	
	maptab = table_create(noc.height, noc.width);
	assert(maptab != NULL);
	
	map = malloc(nprocs*sizeof(unsigned));
	assert(map != NULL);

	table_split(maptab, 0, 0, noc.height, noc.width, nprocs/nclusters);	
	
	/* Map processes. */
	for (i = 0; i < noc.height; i++)
	{
		for (j = 0; j < noc.width; j++)
		{
			c = TABLE(maptab, i, j);
			
			n = list_remove_first(clusters[c].procs);
			
			map[PROCESS(n)->id] = i*noc.width + j;
			
			list_insert(procs, n);
		}
	}
	
	/* Write trace file. */
	fseek(input, 0, SEEK_SET);
	fscanf(input, "%u %u %u %*u %u %u %*u %u\n",
		&start, &xsrc, &ysrc, &xdest, &ydest, &size);
	while (!feof(input))
	{
		src = xsrc*noc.width + ysrc;
		dest = xdest*noc.width + ydest;
			
		fprintf(stdout, "%u %u %u %u %u %u %u %u\n",
			start,
			map[src]/noc.width, map[src]%noc.width, 0,
			map[dest]/noc.width, map[dest]%noc.width, 0,
			size);
        
		fscanf(input, "%u %u %u %*u %u %u %*u %u\n",
			&start, &xsrc, &ysrc, &xdest, &ydest, &size);
	}
	
	free(map);
	table_destroy(maptab);
}
