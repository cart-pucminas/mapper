/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * <matrix/matrix_create.c> - matrix_create() implementation.
 */

#include <stdlib.h>
#include <matrix.h>

/*
 * Creates a matrix.
 */
struct matrix *matrix_create(int width, int height)
{
	struct matrix *m;
	
	/* Invalid argument. */
	if ((width < 0) || (height < 0))
		goto error0;
	
	m = malloc(sizeof(struct matrix));
	
	/* Failed to allocate matrix. */
	if (m == NULL)
		goto error0;
	
	m->elements = calloc(height*width, sizeof(double));
	
	/* Failed to allocate elements. */
	if (m->elements == NULL)
		goto error1;
	
	/* Initialize matrix. */
	m->width = width;
	m->height = height;
	
	return (m);

error1:
	free(m);
error0:
	return (NULL);
}
