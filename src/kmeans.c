/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans.c> - kmeans() implementation.
 */

#include <assert.h>
#include <kmeans.h>
#include <list.h>
#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>

/* Number of processes. */
int nprocs = 0;

/* Processes. */
struct list *procs;

/* Number of clusters. */
int nclusters;

/* Clusters. */
struct cluster *clusters; 

#define PROCESS(n) \
	((struct process *)OBJECT(n))


/*
 * Reads kmeans data from a file.
 */
static int kmeans_read(FILE *input)
{
	int src;
	int i, j;
	int dest;
	int *pid;
	int cost;
	struct matrix *m;
	struct process *proc;
	struct list_node *node;
	struct list *l;
	
	procs = list_create();
	assert(procs != NULL);
	
	/* Get the number of processes. */
	l = list_create();
	assert(l != NULL);
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
			
		pid = malloc(sizeof(int));
		assert(pid != NULL);
		*pid = src;
		node = list_node_create(pid);
		assert(node != NULL);
		list_insert(l, node);
	}
	nprocs = list_length(l);
	/* destroy the list. */
	while (list_length(l))
	{
		node = list_remove_first(l);
		free(OBJECT(node));
		list_node_destroy(node);
	}
	list_destroy(l);

	
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

/*
 * Writes kmeans data to a file.
 */
static void kmeans_write(FILE *output)
{
	int i;
	int processor;
	struct list_node *n;
	
	
	/* Map processes. */
	processor = 0;
	for (i = 0; i < nclusters; i++)
	{
		for (n = list_head(clusters[i].procs); n != NULL; n = list_next(n))
			fprintf(output, "%d %d\n", PROCESS(n)->id, processor++);
		
	}
}

/*
 * Balances number of processes between clusters.
 */
static void kmeans_balance(void)
{
	int i, j;
	double tmp;
	double distance;
	struct list_node *n;
	struct list_node *distant;
	struct cluster *to_insert;
	
	/* Balance number of process between all clusters. */
	for (i = 0; i < nclusters; i++)
	{
		/* Too many processes in this cluster. */
		while (list_length(clusters[i].procs) > clusters[i].size)
		{		
			distant = list_head(clusters[i].procs);
			distance = vector_distance(PROCESS(distant)->traffic, clusters[i].mean);
			
			/* Find the more distant process within the cluster. */
			for (n = list_next(distant); n != NULL; n = list_next(n))
			{
				fflush(stdout);
				tmp = vector_distance(PROCESS(n)->traffic, clusters[i].mean);
				if (tmp > distance)
				{
					distance = tmp;
					distant = n;
				}
			}
			
			to_insert = NULL;
			
			/* Find the best cluster for the process. */
			for (j = 0; j < nclusters; j++)
			{
				/* Skip fully populated clusters. */
				if (list_length(clusters[j].procs) >= clusters[j].size)
					continue;
				
				if (to_insert == NULL)
				{
					to_insert = &clusters[j];
					distance = vector_distance(PROCESS(distant)->traffic, to_insert->mean);
					continue;
				}
				
				tmp = vector_distance(PROCESS(distant)->traffic, clusters[j].mean);
				
				if (tmp < distance)
				{
					distance = tmp;
					to_insert = &clusters[j];
				}
			}
			
			
			/* Insert process in the cluster. */
			list_remove(clusters[i].procs, distant);
			list_insert(to_insert->procs, distant);
		}
	}
}

/*
 * Compute clusters' means.
 */
static int kmeans_compute_means(void)
{
	int i, j;
	int has_changed;
	struct vector *mean;
	struct list_node *n;
	
	has_changed = 0;
	
	
	/* Compute means. */
	for (i = 0; i < nclusters; i++)
	{
		mean = vector_create(nprocs);
		assert(mean != NULL);
			
		/* Compute cluster's mean. */
		for (n = list_head(clusters[i].procs); n != NULL; n = list_next(n))
		{
			for (j = 0; j < nprocs; j++)
				VECTOR(mean, j) += VECTOR(PROCESS(n)->traffic, j);
		}
		for (j = 0; j < nprocs; j++)
			VECTOR(mean, j) /= list_length(clusters[i].procs);
			
		/* Cluster mean has changed. */
		if (!vector_equal(clusters[i].mean, mean))
			has_changed = 1;
		
		vector_destroy(clusters[i].mean);
		clusters[i].mean = mean;	
	}
	
	return (has_changed);
}

/*
 * Populate clusters.
 */
static int kmeans_populate(int mindistance)
{
	int i;
	int too_far;
	struct list_node *n;
	double tmp, distance;
	struct cluster *to_insert;
	
	too_far = 0;
	
	/* Group process in clusters. */
	while (list_length(procs) > 0)
	{
		n = list_remove_first(procs);
		
		/* Look for minimum distance. */
		to_insert = &clusters[0];
		distance = vector_distance(PROCESS(n)->traffic, to_insert->mean);
		for (i = 1; i < nclusters; i++)
		{
			tmp = vector_distance(PROCESS(n)->traffic, clusters[i].mean);
			
			/* New minimum distance found. */
			if (tmp < distance)
			{
				to_insert = &clusters[i];
				distance = tmp;
			}
		}
			
		list_insert(to_insert->procs, n);
		
		/* Found distance is greater than desired. */
		if (distance > mindistance)
			too_far = 1;
	}
	
	return (too_far);
}

/*
 * Depopulate clusters.
 */
static void kmeans_depopulate()
{
	int i;
	struct list_node *n;
	
	/* Depupulate clusters. */
	for (i = 0; i < nclusters; i++)
	{
		while (list_length(clusters[i].procs))
		{
			n = list_remove_first(clusters[i].procs);
			list_insert(procs, n);
		}
	}
}

/*
 *
 */
static void kmeans_create(FILE *input)
{
	int i, j;
	struct list_node *n;
	
	kmeans_read(input);
	
	/* Create clusters. */
	clusters = malloc(nclusters*sizeof(struct cluster));
	assert(clusters != NULL);
	
	/* Initialize clusters. */
	for (i = 0; i < nclusters; i++)
	{
		clusters[i].size = 0;
		clusters[i].procs = list_create();
		assert(clusters[i].procs != NULL);
		clusters[i].mean = vector_create(nprocs);
		assert(clusters[i].mean != NULL);
	}

	/* Populate clusters. */
	for (i = 0, j = 0; i < nprocs; i++, j = (j + 1)%nclusters)
	{
		n = list_remove_first(procs);
		list_insert(clusters[j].procs, n);
		clusters[j].size++;
	}
	
	/* Compute initial means. */
	kmeans_compute_means();
}

/*
 *
 */
static void kmeans_destroy()
{
	int i;
	struct list_node *n;
	
	for (i = 0; i < nclusters; i++)
	{
		while (list_length(clusters[i].procs) > 0)
		{
			n = list_remove_first(clusters[i].procs);
			vector_destroy(PROCESS(n)->traffic);
			free(PROCESS(n));
			list_node_destroy(n);
		}
		list_destroy(clusters[i].procs);
		vector_destroy(clusters[i].mean);
	}
	list_destroy(procs);
	free(clusters);
}

/*
 * 
 */
void kmeans(int _nclusters, int mindistance, FILE *input, FILE *output)
{
	int too_far;
	int has_changed;

	nclusters = _nclusters;
	
	kmeans_create(input);
	
	/* Apply custerization algorithm. */
	do
	{
		kmeans_depopulate();
		
		too_far = kmeans_populate(mindistance);
		
		has_changed = kmeans_compute_means();
		
	} while (too_far && has_changed);
	
	kmeans_balance();
	
	kmeans_write(output);
	
	kmeans_destroy();
}
