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

int tam;



/*

int main(void)
{
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
}

*/

void inicia(node *LISTA)
{
	LISTA->prox = NULL;
	tam=0;
}

int menu(void)
{
	int opt;
	
	printf("Escolha a opcao\n");
	printf("0. Sair\n");
	printf("1. Zerar lista\n");
	printf("2. Exibir lista\n");
	printf("3. Adicionar node no inicio\n");
	printf("4. Adicionar node no final\n");
	printf("5. Escolher onde inserir\n");
	printf("6. Retirar do inicio\n");
	printf("7. Retirar do fim\n");
	printf("8. Escolher de onde tirar\n");
	printf("Opcao: "); scanf("%d", &opt);
	
	return opt;
}

void opcao(node *LISTA, int op)
{
	//node *tmp;
	switch(op){
		//0. Sair
		case 0:
			libera(LISTA);
			break;
		
		//1. Zerar lista
		case 1:
			libera(LISTA);
			inicia(LISTA);
			break;

		//2. Exibir lista
		case 2:
			exibe(LISTA);
			break;

		//3. Adicionar node no inicio
		case 3:
			insereInicio(LISTA);
			break;		

		//4. Adicionar node no final
		case 4:
			insereFim(LISTA);
			break;
		
		//5. Escolher onde inserir
		case 5:
			insere(LISTA);
			break;
		
		//6. Retirar do inicio
		case 6:
		//	tmp= retiraInicio(LISTA);
		//	printf("Retirado: %3d\n\n", tmp->num);
			break;
		//7. Retirar do fim
		case 7:
	//		tmp= retiraFim(LISTA);
		//	printf("Retirado: %3d\n\n", tmp->num);
			break;
		
		//8. Escolher de onde tirar			
		case 8:
	//		tmp= retira(LISTA);
		//	printf("Retirado: %3d\n\n", tmp->num);
			break;
		
		default:
			printf("Comando invalido\n\n");
	}

//Desalocar tmp?
	//free(tmp);
}

int vazia(node *LISTA)
{
	if(LISTA->prox == NULL)
		return 1;
	else
		return 0;
}

node *aloca()
{
	node *novo=(node *) malloc(sizeof(node));
	if(!novo){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
	//	printf("Novo elemento: "); scanf("%d", &novo->num);
		return novo;
	}
}

node *alocaAddr(int addr, int tid)
{
	node *novo=(node *) malloc(sizeof(node));
	if(!novo){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
		//printf("Novo elemento: "); scanf("%d", &novo->num);
		int count=0; 
		for(count=1; count < 12; count++){
				novo->acessos[count] = 0;
		}
		novo->acessos[tid] = 1;
		novo->addr = addr;
		
		return novo;
	}
}

void insereFim(node *LISTA)
{
	node *novo=aloca();
	novo->prox = NULL;
	
	if(vazia(LISTA))
		LISTA->prox=novo;
	else{
		node *tmp = LISTA->prox;
		
		while(tmp->prox != NULL)
			tmp = tmp->prox;
		
		tmp->prox = novo;
	}
	tam++;
}

void insereInicio(node *LISTA)
{
	node *novo=aloca();	
	node *oldHead = LISTA->prox;
	
	LISTA->prox = novo;
	novo->prox = oldHead;
	
	tam++;
}

void insereInicioAddr(node *LISTA, int addr, int tid, int tipo)
{
	node *novo=alocaAddr(addr, tid);	
	node *oldHead = LISTA->prox;
	
	LISTA->prox = novo;
	novo->prox = oldHead;
	
	tam++;
}

void insereAcesso(node *LISTA, int addr, int tid, int tipo)
{
	
	if(LISTA->prox == NULL){ //??
		//lista está vazia
		insereInicioAddr(LISTA, addr, tid, tipo);
	}
	else{
		node *atual = LISTA->prox;
		int bloco = 0;
		while (!bloco){
			if (atual->addr == addr){
				atual->acessos[tid] += 1;
				bloco = 1; 
			}else{
				//Se fim da lista
				if (atual->prox == NULL){
					//Addr não encontrado
					insereInicioAddr(LISTA, addr, tid, tipo);
					bloco = 1;
				}else{ atual = atual->prox; }
			}
		}
	}	
}


void exibe(node *LISTA)
{
	system("clear");
	if(vazia(LISTA)){
		printf("Lista vazia!\n\n");
		return ;
	}
	
	node *tmp;
	tmp = LISTA->prox;
	printf("Lista:");
	while( tmp != NULL){
	//	printf("%5d", tmp->num);
		tmp = tmp->prox;
	}
	printf("\n        ");
	int count;
	for(count=0 ; count < tam ; count++)
		printf("  ^  ");
	printf("\nOrdem:");
	for(count=0 ; count < tam ; count++)
		printf("%5d", count+1);
	
		
	printf("\n\n");
}

//Esvaziar lista
void libera(node *LISTA)
{
	if(!vazia(LISTA)){
		node *proxNode,
			  *atual;
		
		atual = LISTA->prox;
		while(atual != NULL){
			proxNode = atual->prox;
			free(atual);
			atual = proxNode;
		}
	}
}

void insere(node *LISTA)
{
	int pos,
		count;
	printf("Em que posicao, [de 1 ate %d] voce deseja inserir: ", tam);
	scanf("%d", &pos);
	
	if(pos>0 && pos <= tam){
		if(pos==1)
			insereInicio(LISTA);
		else{
			node *atual = LISTA->prox,
				 *anterior=LISTA; 
			node *novo=aloca();
				 
			for(count=1 ; count < pos ; count++){
					anterior=atual;
					atual=atual->prox;
			}
			anterior->prox=novo;
			novo->prox = atual;
			tam++;
		}
			
	}else
		printf("Elemento invalido\n\n");		
}

/*
void insereAcesso(node *LISTA, int addr, int tid)
{
	
	if(LISTA->prox == NULL){
		//lista está vazia
		insereInicioAddr(LISTA, addr, tid);
	}
	else{
		node *atual = LISTA->prox;
		int bloco = 0;
		while (!bloco){
			if (atual->addr == addr){
				atual->acessos[tid] += 1;
				bloco = 1; 
			}else{
				//Se fim da lista
				if (atual->prox == NULL){
					//Addr não encontrado
					insereInicioAddr(LISTA, addr, tid);
					bloco = 1;
				}else{ atual = atual->prox; }
			}
		}
	}	
}

*/

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

