/*
 * Copyright(C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <vector.h> - Vector library.
 */

#ifndef VECTOR_H_
#define VECTOR_H_

	/*
	 * Vector.
	 */
	struct vector
	{
		int size;         /* Size.     */
		double *elements; /* Elements. */
	};

	/*
	 * Creates a vector.
	 */
	extern struct vector *vector_create(int n);
	
	/*
	 * Destroys a vector.
	 */
	extern void vector_destroy(struct vector *v);
	
	/*
	 * Computes the euclidean distance between two points.
	 */
	extern double vector_distance(struct vector *a, struct vector *b);
	
	/*
	 * Tests if two vectors are equal.
	 */
	extern int vector_equal(struct vector *a, struct vector *b);

	/*
	 * Returns the element [i] in a vector.
	 */
	#define VECTOR(v, i) \
		((v)->elements[(i)])

#endif /* VECTOR_H_ */
