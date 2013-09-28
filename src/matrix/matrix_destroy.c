/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <matrix/matrix_destroy.c> - matrix_destroy() implementation.
 */

#include <stdlib.h>
#include <matrix.h>
 
/*
 * Destroys a matrix.
 */
void matrix_destroy(struct matrix *m)
{
	free(m->elements);
	free(m);
}
