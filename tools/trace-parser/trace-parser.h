/*
 * Copyright(C) 2015 Amanda Amorim <amandamp.amorim@gmail.com>
 *                   Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Mapper.
 * 
 * Mapper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mapper. If not, see <http://www.gnu.org/licenses/>.
 */

 
#ifndef TRACE-PARSER_H_
#define TRACE-PARSER_H_

#include <stdlib.h>

#include <mylib/util.h>
#include <mylib/object.h>
#include <mylib/matrix.h>




extern void trace_read(struct cache *, FILE *, int);
extern void matrix_generate(FILE *, struct matrix *);

extern void cache_update2(struct cache *, object_t);


#endif /* TRACE-PARSER_H_ */
