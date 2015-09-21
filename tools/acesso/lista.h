struct Node{
//	int num;
	int addr;
	int acessos [12];
	int flag;
	int idade;
	struct Node *prox;
}; 
typedef struct Node node;


void inicia(node *LISTA);
int menu(void);
void opcao(node *LISTA, int op);
node *criaNo();
void insereFim(node *LISTA);
void insereInicio(node *LISTA);
void exibe(node *LISTA);
void libera(node *LISTA);
void insere (node *LISTA);
node *retiraInicio(node *LISTA);
node *retiraFim(node *LISTA);
node *retira(node *LISTA);

void insereAcesso(node *LISTA, int addr, int tid, int tipo);


