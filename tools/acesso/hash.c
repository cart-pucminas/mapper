/*
 * lista.c
 * 
 * Copyright 2015 Amanda <amanda@thor-pc>
 * Retirado e adaptado de:
 * http://www.cprogressivo.net/2013/10/Como-fazer-uma-lista-em-C.html
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
#include <stdlib.h>
#include "lista.h"

#define tamHash 256

typedef node* Hash[tamHash];

int tamListas[tamHash];

int tam;



void inicializaHash(Hash tab);
int funcaoHash(int addr);
//void insereAcesso(node *LISTA, int addr, int tid, int tipo);
//void insereInicioAddr(node *LISTA, int addr, int tid, int tipo);

int main(void)
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
//	char nomeArq[] = "trace0.txt";
	int addr=1;
	int tid=0;
//	int bytes=0;
	int tipo=1;
	
	int endTab=0;
	//inicializar tabela Hash
	inicializaHash(tab);
	//Abrir arquivos dos traces
	//FILE* arquivo;
	//arquivo = fopen(nomeArq, "r");
	
	//Ler arquivos
	
	//Ler addr
	
	endTab = funcaoHash(addr);
	
	int x = 5;
	int y = 4;
	 
	 teste(x,y);
	 
	//insereAcesso(tab[endTab], addr, tid, tipo);
	
	
	
	return 0;
}

/*O procedimento inicializaHash recebe como parametro uma variavel do tipo
 Hash e sua funcao e que todas as posicoes da tab se tornem nulas*/
void inicializaHash(Hash tab)
{
	int i;
	for(i = 0; i < tamHash; i++)
	{
		tab[i] = NULL;
		tamListas[i] = 0;
	}
}

int funcaoHash(int addr)
{
  return(addr%tamHash);
}

void inicia(node *LISTA)
{
	LISTA->prox = NULL;
	tam=0;
}




node *retiraInicio(node *LISTA)
{
	if(LISTA->prox == NULL){
		printf("Lista ja esta vazia\n");
		return NULL;
	}else{
		node *tmp = LISTA->prox;
		LISTA->prox = tmp->prox;
		tam--;
		return tmp;
	}
				
}

node *retiraFim(node *LISTA)
{
	if(LISTA->prox == NULL){
		printf("Lista ja vazia\n\n");
		return NULL;
	}else{
		node *ultimo = LISTA->prox,
			 *penultimo = LISTA;
			 
		while(ultimo->prox != NULL){
			penultimo = ultimo;
			ultimo = ultimo->prox;
		}
			 
		penultimo->prox = NULL;
		tam--;
		return ultimo;		
	}
}

node *retira(node *LISTA)
{
	int opt,
		count;
	printf("Que posicao, [de 1 ate %d] voce deseja retirar: ", tam);
	scanf("%d", &opt);
	
	if(opt>0 && opt <= tam){
		if(opt==1)
			return retiraInicio(LISTA);
		else{
			node *atual = LISTA->prox,
				 *anterior=LISTA; 
				 
			for(count=1 ; count < opt ; count++){
				anterior=atual;
				atual=atual->prox;
			}
			
		anterior->prox=atual->prox;
		tam--;
		return atual;
		}
			
	}else{
		printf("Elemento invalido\n\n");
		return NULL;
	}
}

