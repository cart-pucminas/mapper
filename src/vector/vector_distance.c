/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <vector/vector_distance.c> - vector_distance() implementation.
 */

#include <math.h>
#include <vector.h>
#include <stdio.h>

/*
 * Computes the euclidean distance between two points.
 */
double vector_distance(struct vector *a, struct vector *b)
{
	int i;
	double distance;
	
	/* Vectors do not agree on size. */
	if (a->size != b->size)
		return (-1);

	distance = 0;

	/* Computes the euclidean distance. */
	for (i = 0; i < a->size; i++)
		distance +=  pow(VECTOR(a, i) - VECTOR(b, i), 2);
	distance = sqrt(distance);
	
	return (distance);
}

