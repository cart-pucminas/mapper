/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_read.c> - kmeans_read() implementation.
 */

#include <assert.h>
#include <list.h>
#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>
#include "kmeans.h"

/*
 * Get the number of processes.
 */
static void get_procnum(FILE *input)
{
	int src;
	int *pid;
	struct list *l;
	struct list_node *node;
	
	l = list_create();
	assert(l != NULL);

	/* Read all data. */
	while (fscanf(input, "%d %*d %*d", &src) != EOF)
	{
		/* Check if process is already in the list. */
		for (node = list_head(l); node != NULL; node = list_next(node))
		{
			/* Found. */
			if (*((int *)OBJECT(node)) == src)
				break;
		}
		
		/* This process is already in the list. */
		if (node != NULL)
			continue;
		
		/* Insert process in the list.*/
		pid = malloc(sizeof(int));
		assert(pid != NULL);
		*pid = src;
		node = list_node_create(pid);
		assert(node != NULL);
		list_insert(l, node);
	}
	
	nprocs = list_length(l);
	
	/* Destroy the list. */
	while (list_length(l))
	{
		node = list_remove_first(l);
		free(OBJECT(node));
		list_node_destroy(node);
	}
	list_destroy(l);
}

/*
 * Reads kmeans() data from a file.
 */
int kmeans_read(FILE *input)
{
	int src;
	int i, j;
	int dest;
	int cost;
	struct matrix *m;
	struct process *proc;
	struct list_node *node;
	
	procs = list_create();
	assert(procs != NULL);
	
	get_procnum(input);
	
	assert((nprocs & 2) == 0);
	assert((noc.height*noc.width) == nprocs);

	m = matrix_create(nprocs, nprocs);
	assert(m != NULL);
	
	/* Read communication matrix. */
	fseek(input, 0, SEEK_SET);
	while ((fscanf(input, "%d %d %d", &src, &dest, &cost) != EOF))
		MATRIX(m, src, dest) = cost;
	
	/* Create processes. */
	for (i = 0; i < nprocs; i++)
	{
		proc = malloc(sizeof(struct process));
		assert(proc != NULL);
		
		proc->traffic = vector_create(nprocs);
		assert(proc->traffic);
		
		proc->id = i;
		for (j = 0; j < nprocs; j++)
			VECTOR(proc->traffic, j) = MATRIX(m, i, j);
		
		node = list_node_create(proc);
		assert(node != NULL);
		
		list_insert(procs, node);
	}
	
	matrix_destroy(m);
	
	return (0);
}
