#ifndef GERACAO_DE_CODIGO_H
#define GERACAO_DE_CODIGO_H

#include "ast.h"
#include "verificacao_de_tipos.h"
#include "lista.h"
#include "mapa.h"
#include <stdbool.h>

// Memória usada para o compilador armazenar seus dados auxiliares
// É como se fossem 1024 registradores de um byte livres para o compilador fazer o que quiser
#define INICIO_MEMORIA_AUXILIAR 0
#define FIM_MEMORIA_AUXILIAR 1023
// Memória disponível para o programa
#define INICIO_MEMORIA_DISPONIVEL 1024
#define FIM_MEMORIA_DISPONIVEL 4998975
// Memória usada para se comunicar com as extensões da Máquina virtual
#define INICIO_MEMORIA_EXTENSAO 4998976
#define FIM_MEMORIA_EXTENSAO 4999999

char* gerarAssembly(programa_t *programa);

#endif /* GERACAO_DE_CODIGO_H */
