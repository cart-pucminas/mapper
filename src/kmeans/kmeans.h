/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <kmeans/kmeans.h> - Private kmeans() library.
 */

#ifndef _KMEANS_H_
#define _KMEANS_H_
	
	#include <stdio.h>
	
	/*
	 * Process.
	 */
	struct process
	{
		int id;                 /* Process ID.            */
		struct vector *traffic; /* Communication traffic. */
	};
	
	/*
	 * Cluster of processes.
	 */
	struct cluster
	{
		int size;            /* Maximum number of process in the cluster. */
		struct list *procs;  /* Processes in the cluster.                 */
		struct vector *mean; /* Processes' comunication traffic mean.     */
	};
	
	/* Number of processes. */
	extern int nprocs;

	/* Processes. */
	extern struct list *procs;

	/* Number of clusters. */
	extern int nclusters;

	/* Clusters. */
	extern struct cluster *clusters; 

	/*
	 * Initializes internal data structures.
	 */
	extern void kmeans_init(FILE *input, int _nclusters);
	
	/*
	 * Destroys internal data structures.
	 */
	extern void kmeans_destroy(void);
	
	/*
	 * Populates clusters.
	 */
	extern int kmeans_populate(int mindistance);
	
	/*
	 * Depopulate clusters.
	 */
	extern void kmeans_depopulate(void);

	/*
	 * Reads kmeans() data from a file.
	 */
	extern int kmeans_read(FILE *input);

	/*
	 * Writes kmeans() result in a file.
	 */
	extern void kmeans_write(FILE *output);
	
	/*
	 * Compute the means of the clusters.
	 */
	extern int kmeans_compute(void);
	
	/*
	 * Balances number of processes between clusters.
	 */
	extern void kmeans_balance(void);
	
	/*
	 * Handful type cast.
	 */
	#define PROCESS(n) \
		((struct process *)OBJECT(n))
	
#endif /* _KMEANS_H_ */
