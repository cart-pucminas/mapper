/*
 * main.c
 * 
 * Copyright 2015 Amanda <amanda@thor-pc>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
//#include "lista.h"
#include "hash.h"

int main(int argc, char **argv)
{
	
	
	

	/*
	//Criar lista
	node *LISTA = (node *) malloc(sizeof(node));
	if(!LISTA){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
	
	//Iniciar lista
	inicia(LISTA);
	
	int opt;
	
	do{
		opt=menu();
		opcao(LISTA,opt);
	}while(opt);

	//Desalocar lista
	free(LISTA);
	return 0;
	}
	*/
	
	Hash tab;
	allocHash(tab);
	inicializaHash(tab);
	printf("OK!\n");
	int endTab = 0;
	
	int addr=1; //Qual deve ser o tipo 

	int tid=0;

//	int bytes=0;

	int tipo=1; //r=1, w=2
	
	endTab = funcaoHash(addr);
	
	printf("\naddr= %d endTab= %d\n", addr, endTab);
	
	insereAcesso(tab[endTab], addr, tid, tipo);
	insereAcesso(tab[endTab], addr, tid, tipo);
	
	addr = 257;
	endTab = funcaoHash(addr);
	
	printf("\naddr= %d endTab= %d\n", addr, endTab);
	
	insereAcesso(tab[endTab], addr, tid, tipo);
	
	addr = 513;
	endTab = funcaoHash(addr);
	
	printf("\naddr= %d endTab= %d\n", addr, endTab);
	
	insereAcesso(tab[endTab], addr, tid, tipo);
	
	printf("OK Fim !\n");
 
	
	
//	node *LISTA = (node *) malloc(sizeof(node));



//	char nomeArq[] = "trace0.txt";

	//int addr=1; //Qual deve ser o tipo 

	//int tid=0;

//	int bytes=0;

	//int tipo=1; //r=1, w=2
	
	

//	int endTab=0;
	//inicializar tabela Hash
//	inicializaHash(tab);
	//Abrir arquivos dos traces
	//FILE* arquivo;
	//arquivo = fopen(nomeArq, "r");
	
	//Ler arquivos
	
	//Ler addr
	
	//endTab = funcaoHash(addr);
	
 
	//insereAcesso(tab[endTab], addr, tid, tipo);
	
	
	return 0;
}

	

