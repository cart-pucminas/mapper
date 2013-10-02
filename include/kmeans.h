/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <kmeans.h> - k-means() library.
 */

#ifndef KMEANS_H_
#define KMEANS_H_
	
	#include <stdio.h>
	
	/*
	 * k-means algorithm clusterization.
	 */
	extern void kmeans(int nclusters, int mindistance, FILE *input, FILE *output);
	
#endif /* KMEANS_H_ */
