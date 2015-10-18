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

#include <inttypes.h> 
#include <string.h>

#include <mylib/util.h>
#include <mylib/cache.h>
#include <mylib/object.h>
#include <mylib/matrix.h>
#include "access.h"


 void trace_read(struct cache *c, FILE * trace, int th)
 {
	char rw;       /* Access' type.    */
	int size;      /* Access' size.    */
	uint64_t addr; /* Access' address. */	
	
	/* Bad types. */
	if (sizeof(key_t) < sizeof(uint64_t))
		error("bad types");
	
	while (fscanf(trace, "%c%*c%d%*c%" PRIx64 "%*d", &rw, &size, &addr) != EOF)
	{
		if (rw != 'R' && rw != 'W')
			continue;
			
		for (uint64_t x = addr; x < (addr+size); x++)
		{
			struct access *accessMem;
				
			accessMem = cache_get(c, x);
								
			/* Update access. */
			if (accessMem != NULL)
			{
				accessMem->access[th] += (rw == 'W') ? 2 : 1;
				cache_update(c, accessMem);
				continue;
			}

			/* Create new access. */
			accessMem = access_create();
			accessMem->addr = x;
			memset(accessMem->access, 0, sizeof(QTD_THREADS*sizeof(int)));
			accessMem->access[th] += (rw == 'W') ? 2 : 1;
			
			cache_insert(c, accessMem);
		}
	}
}


void matrix_generate(FILE *swp, struct matrix *m){
	
	int x;
	int y;
	int e;
	
	struct access *a;
	a = smalloc(sizeof(struct access));
	
	//Ler acessos gravados na swap
	a = access_info.read(swp);
		
	while(!feof(swp)){


		if (a==NULL){
				fprintf(stderr,"\nErro ao ler o acesso do arquivo\n");
				break;
		}
		
		//fprintf(stderr,"\na->addr=%x, a->access[0]=%d, a->access[1]=%d, a->access[2]=%d \n",a->addr, a->access[0], a->access[1], a->access[2]);


		//Verificar os compartilhamentos entre cada par de threads
		for(x=0; x <QTD_THREADS; x++ ){
			for(y=0; y<QTD_THREADS; y++){
				if(a->access[x] != 0 && a->access[y] != 0){
					//Obter a quantidade de bytes compartilhados pelas
					//threads X, Y até o momento
					e = matrix_get(m, x, y);
					//fprintf(stderr, "\nx=%d, y=%d, e=%d\n", x, y, e);
					if (a->access[x] <= a->access[y])
						e += a->access[x];
					else
						e += a->access[y];	
					
					matrix_set(m, x, y, e);	
				
				}			
			}
		}
		
		//Ler acessos gravados na swap
		a = access_info.read(swp);
		
	}
	
	
}
