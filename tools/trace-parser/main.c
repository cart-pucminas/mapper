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

#include <stdlib.h>
#include <stdio.h>
#include <mylib/object.h>
#include <mylib/hash.h>
#include <mylib/cache.h>
#include <mylib/matrix.h>

#include "access.h"
#include "trace-parser.h"




int main(int argc, char **argv)
{
	unsigned size_cache = 256;
	int x, y;
	char name_trace[80];
	
	//Abrir arquivo de swap para gravação e leitura
	FILE * swp;
	swp = fopen("swap.swp", "w+");
	
	if(swp == NULL){
		printf("Arquivo de swp não pode ser aberto");
		return(EXIT_FAILURE);
	}
	
	//Criar a Cache
	struct cache * c = cache_create( &access_info, swp, size_cache);

	FILE * trace;
	//Ler todos os traces e armazenar os acessos na cache
	for(x=0; x<QTD_THREADS; x++){
		
		sprintf(name_trace,"saida-%d.out", x);
		trace = fopen(name_trace, "r");
		if(trace == NULL){
			printf("Arquivo de trace não pode ser aberto");
			return(EXIT_FAILURE);
		}
		
		//Ler o trace gravar na cache
		trace_read(c, trace, x);
		
		//Fechar arquivo de trace
		fclose(trace);
	}
	
	//Descarregar toda a cache no arquivo de swap
	cache_flush(c);	
	
	//Ferchar arquivo de swap
	fclose(swp);
	
	//Abrir arquivo de swap para leitura 
	swp = fopen("swap.swp", "r");
	
	
	
	//Criar matriz de compatilhamento
	struct matrix * m = matrix_create(QTD_THREADS, QTD_THREADS);
	
	//Gravar os acessos da swap na matriz de comparitlhamento 
	matrix_generate(swp, m);
	
	//Fechar arquivo de swap
	fclose(swp);
	
	//Gravar a matrix de compartilhamento em um arquivo
	FILE * matrix_share; 
	swp = fopen("matrix-share.out", "w");
	if(matrix_share == NULL){
		printf("Arquivo para gravação da matrix não pode ser aberto");
		return(EXIT_FAILURE);
	}
	
	for(x=0; x<QTD_THREADS; x++){
		for(y=0; y<QTD_THREADS; x++){
			fprintf(matrix_share, "%d;%d;%d\n", x, y, matrix_get(m, x, y));	
				
				
		}
	}
	
	
	
	

	return (EXIT_SUCCESS);
}

	

