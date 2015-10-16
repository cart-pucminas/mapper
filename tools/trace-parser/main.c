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
	
	unsigned size_cache = 40;
	int x, y;
	char name_trace[80];
	
	//Abrir arquivo de swap para gravação e leitura
	FILE * swp;
	
	//swp = fopen("~/teste-mapper/MG-W/saidas/swap.swp", "w+");
	swp = fopen("/home/amanda/teste-mapper-reduzido/MG-W/saidas/swap.swp", "w+");

	if(swp == NULL){
		printf("Arquivo de swp não pode ser aberto");
		return(EXIT_FAILURE);
	}

	
	//Criar a Cache
	struct cache * c = cache_create( &access_info, swp, size_cache);

	printf("\nCache criada\n");
	
	FILE * trace;
	//Ler todos os traces e armazenar os acessos na cache
	for(x=0; x<QTD_THREADS; x++){
		
		//sprintf(name_trace,"~/teste-mapper/MG-W/out.tid%d.mem.out", x);
		sprintf(name_trace,"/home/amanda/teste-mapper-reduzido/MG-W/out.tid%d.mem.out", x);
		trace = fopen(name_trace, "r");
		if(trace == NULL){
			fprintf(stderr, "\nArquivo de trace não pode ser aberto\n");
			return(EXIT_FAILURE);
		}
		
		//Ler o trace gravar na cache
		printf("\nLendo o trace: %s\n", name_trace);
		trace_read(c, trace, x);
		
		//Fechar arquivo de trace
		fclose(trace);
	}
	
	//Descarregar toda a cache no arquivo de swap
	fprintf(stderr,"\nCache flush\n");
	cache_flush(c);	

	//Ferchar arquivo de swap
	//fclose(swp);
	
	//Abrir arquivo de swap para leitura 
	//swp = fopen("~/teste-mapper/MG-W/saidas/swap.swp", "r");
	//swp = fopen("/home/amamda/teste-mapper-reduzido/MG-W/saidas/swap.swp", "r");
	
	fprintf(stderr,"\nColocar ponteiro do arquivo de swap no início\n");
	fseek(swp, 0, SEEK_SET); 

	//Criar matriz de compatilhamento
	struct matrix * m = matrix_create(QTD_THREADS, QTD_THREADS);
	

	//Gravar os acessos da swap na matriz de comparitlhamento 
	matrix_generate(swp, m);
	

	//Fechar arquivo de swap
	fclose(swp);
	
	//Gravar a matrix de compartilhamento em um arquivo
	FILE * matrix_shared; 
	//matrix_shared = fopen("~/teste-mapper/MG-W/saidas/matrix-shared.out", "w");
	matrix_shared = fopen("/home/amanda/teste-mapper-reduzido/MG-W/saidas/matrix-shared.out", "w");
	
	if(matrix_shared == NULL){
		fprintf(stderr, "\nArquivo para gravação da matrix não pode ser aberto\n");
		return(EXIT_FAILURE);
	}
	
	int e=0;
	for(x=0; x<QTD_THREADS; x++){
		for(y=0; y<QTD_THREADS; y++){
			e=matrix_get(m, x, y);
		//	fprintf(stderr,"\nx=%d y=%d e=%d\n", x, y, e);
			fprintf(matrix_shared, "%d;%d;%d\n", x, y, e );	
				
		}
	}
	
	
	fclose(matrix_shared);
		
	return (EXIT_SUCCESS);
}

	

