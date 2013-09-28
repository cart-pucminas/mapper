/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <matrix.h> - Matrix library.
 */

#ifndef MATRIX_H_
#define MATRIX_H_

	/*
	 * Matrix.
	 */
	struct matrix
	{
		int width;        /* Width.    */
		int height;       /* Height.   */
		double *elements; /* Elements. */
	};
	
	/*
	 * Creates a matrix.
	 */
	extern struct matrix *matrix_create(int height, int width);
	
	/*
	 * Destroys a matrix.
	 */
	extern void matrix_destroy(struct matrix *m);
	
	/*
	 * Returns the element [i, j] in a matrix.
	 */
	#define MATRIX(m, i, j) \
		((m)->elements[(i)*(m)->width + (j)])
	 
#endif /* MATRIX_H_ */
