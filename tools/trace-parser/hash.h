
#include "lista.h"

#define tamHash 256

typedef node* Hash[tamHash];

int tamListas[tamHash];

int tam;

void allocHash (Hash tab);
void inicializaHash(Hash tab);
int funcaoHash(int addr);
