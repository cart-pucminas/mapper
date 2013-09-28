/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 */

#ifndef KMEANS_H_
#define KMEANS_H_
	
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
		int size;            /* Number of process in the cluster.     */
		struct list *procs;  /* Processes in the cluster.             */
		struct vector *mean; /* Processes' comunication traffic mean. */
	};
	
#endif /* KMEANS_H_ */
