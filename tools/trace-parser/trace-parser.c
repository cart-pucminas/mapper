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
#include <mylib/cache.h>
#include <mylib/object.h>
#include "access.h"




 void trace_read(struct cache *c, FILE * trace, int th){
	
	char rw;
	int size;
	uint64_t addr;	
	char line[80];
	char ponto1; //recebe o ponto e vírgula
	char ponto2; //recebe o ponto e vírgula
	
	struct access *accessMem;
	
	while( (fscanf(trace,"%s", line))!=EOF ){
				
			sscanf(line, "%c%c%d%c%" PRIx64 "%*d", &rw, &ponto1, &size, &ponto2, &addr);	
	
			printf("\n rw = %c\n", rw);
			printf("\n size = %d\n", size);
			printf("\n addr = %" PRIx64 "\n", addr);
			
			//Buscar o access do addr lido na cache
			accessMem = (struct access*) cache_get(c, addr);
			
			if (accessMem !=NULL){
				//Acrescentar o acesso lido 
				accessMem->access[th] +=size;
				//Atualizar a cache
				cache_update(c, accessMem);
			}else{
				//Criar um novo access
				accessMem = access_create();
				accessMem->addr = addr;
				accessMem->access[th]  += size;
				//Inserir na cache
				cache_insert(c, accessMem);
			}
		}
}
