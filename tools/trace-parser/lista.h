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
 
#ifndef LIST_H_
#define LIST_H_

	/**
	 * @brief List node.
	 */
	struct Node
	{
		int addr;
		int acessos [12];
		int flag;
		int idade;
		struct Node *prox;
	};

	/* Forward definitions. */
	struct Node *aloca();
	struct Node *alocaAddr(int addr, int tid);
	void inicia(struct Node *LISTA);
	int menu(void);
	void opcao(struct Node *LISTA, int op);
	struct Node *criaNo();
	void insereFim(struct Node *LISTA);
	void insereInicio(struct Node *LISTA);
	void exibe(struct Node *LISTA);
	void libera(struct Node *LISTA);
	void insere (struct Node *LISTA);
	struct Node *retiraInicio(struct Node *LISTA);
	struct Node *retiraFim(struct Node *LISTA);
	struct Node *retira(struct Node *LISTA);

	void insereAcesso(struct Node *LISTA, int addr, int tid, int tipo);

#endif /* LIST_H_ */


