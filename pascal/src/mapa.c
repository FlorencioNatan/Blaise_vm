#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <graphviz/gvc.h>
#include "mapa.h"

no_t* buscarChaveNoMapa(char *chave, mapa_t *mapa) {
	no_t* no = mapa->nil;
	no_t* proximo_no = mapa->raiz;

	while (proximo_no != mapa->nil) {
		if (strcmp(chave, proximo_no->chave) < 0) {
			proximo_no = proximo_no->esquerda;
		} else if (strcmp(chave, proximo_no->chave) > 0) {
			proximo_no = proximo_no->direita;
		} else {
			no = proximo_no;
			proximo_no = mapa->nil;
		}
	}

	return no;
}

no_t* localizarPaiNoMapa(no_t *no, mapa_t *mapa) {
	no_t* pai = mapa->raiz;
	no_t* proximo_no = pai;

	while (proximo_no != mapa->nil) {
		pai = proximo_no;
		if (strcmp(no->chave, pai->chave) < 0) {
			proximo_no = pai->esquerda;
		} else if (strcmp(no->chave, pai->chave) > 0) {
			proximo_no = pai->direita;
		} else {
			proximo_no = mapa->nil;
		}
	}

	return pai;
}

no_t* localizarTioNoMapa (no_t *no, mapa_t *mapa) {
	no_t *pai = no->pai;
	if (pai->pai == mapa->nil) {
		return mapa->nil;
	}

	if (strcmp(pai->chave, pai->pai->chave) < 0) {
		return pai->pai->direita;
	} else {
		return pai->pai->esquerda;
	}
}

void rotacionarEsquerda(no_t *x, mapa_t *mapa) {
	no_t *y = x->direita;
	x->direita = y->esquerda;
	if (y->esquerda != mapa->nil) {
		y->esquerda->pai = x;
	}
	y->pai = x->pai;

	if (x->pai == mapa->nil) {
		mapa->raiz = y;
	} else if (x == x->pai->esquerda) {
		x->pai->esquerda = y;
	} else {
		x->pai->direita = y;
	}

	y->esquerda = x;
	x->pai = y;
}

void rotacionarDireita(no_t *x, mapa_t *mapa) {
	no_t *y = x->esquerda;
	x->esquerda = y->direita;
	if (y->direita != mapa->nil) {
		y->direita->pai = x;
	}
	y->pai = x->pai;

	if (x->pai == mapa->nil) {
		mapa->raiz = y;
	} else if (x == x->pai->direita) {
		x->pai->direita = y;
	} else {
		x->pai->esquerda = y;
	}

	y->direita = x;
	x->pai = y;
}

void corrigirViolacoesInsercaoNoMapa (no_t *no, mapa_t *mapa) {
	while (no->pai->cor == COR_VERMELHO) {
		no_t *tio = localizarTioNoMapa(no, mapa);
		no_t *pai = no->pai;
		no_t *avo = pai->pai;

		int cor_tio = COR_PRETO;
		if (tio != mapa->nil && tio->cor == COR_VERMELHO) {
			cor_tio = COR_VERMELHO;
		}

		if (cor_tio == COR_VERMELHO) {
			tio->cor = COR_PRETO;                              // Caso 1
			pai->cor = COR_PRETO;                              // Caso 1

			avo->cor = COR_VERMELHO;                           // Caso 1
			no = avo;                                          // Caso 1
			continue;
		}

		if (cor_tio == COR_PRETO) {
			if (strcmp(pai->chave, avo->esquerda->chave) == 0) {
				if (strcmp(no->chave, pai->direita->chave) == 0) { // Filho direito de um filho esquerdo
					no = pai;                                  // Caso 2
					rotacionarEsquerda(no, mapa);              // Caso 2
				}
				no->pai->cor = COR_PRETO;                      // Caso 3 // Ambos os filhos de um filho esquerdo
				no->pai->pai->cor = COR_VERMELHO;              // Caso 3
				rotacionarDireita(no->pai->pai, mapa);         // Caso 3
			} else {
				if (strcmp(no->chave, pai->esquerda->chave) == 0) {  // Filho esquerdo de um filho direito
					no = pai;                                  // Caso 4
					rotacionarDireita(no, mapa);               // Caso 4
				}
				no->pai->cor = COR_PRETO;                      // Caso 5 // Ambos os filhos de um filho direito
				no->pai->pai->cor = COR_VERMELHO;              // Caso 5
				rotacionarEsquerda(no->pai->pai, mapa);        // Caso 5
			}
		}
	}
	mapa->raiz->cor = COR_PRETO;
}

mapa_t* addNodeNoMapa(no_t *no, mapa_t *mapa) {
	if (mapa == NULL) {
		no->cor = COR_PRETO;

		no_t *nil = malloc(sizeof(no_t));
		mapa_t *novo_mapa = malloc(sizeof(mapa_t));
		no->pai = nil;
		no->esquerda = nil;
		no->direita = nil;
		novo_mapa->raiz = no;
		novo_mapa->nil = nil;
		return novo_mapa;
	}

	no->cor = COR_VERMELHO;
	no->esquerda = mapa->nil;
	no->direita = mapa->nil;
	no_t *pai = localizarPaiNoMapa(no, mapa);
	if (strcmp(no->chave, pai->chave) < 0) {
		pai->esquerda = no;
		no->pai = pai;
	} else if (strcmp(no->chave, pai->chave) > 0) {
		pai->direita = no;
		no->pai = pai;
	} else {
		pai->tipo = no->tipo;
		pai->valor = no->valor;
	}

	if (pai->cor == COR_VERMELHO) {
		corrigirViolacoesInsercaoNoMapa(no, mapa);
	}

	return mapa;
}

mapa_t* addStringNoMapa(char *chave, char *valor, mapa_t *mapa) {
	no_t *novoNo = malloc(sizeof(no_t));
	novoNo->tipo = TC_STRING;
	novoNo->chave = malloc(sizeof(char) * (strlen(chave) + 1));
	novoNo->valor.strVal = malloc(sizeof(char) * (strlen(valor) + 1));
	strcpy(novoNo->chave, chave);
	strcpy(novoNo->valor.strVal, valor);

	return addNodeNoMapa(novoNo, mapa);
}

mapa_t* addIntNoMapa(char *chave, int valor, mapa_t *mapa) {
	no_t *novoNo = malloc(sizeof(no_t));
	novoNo->tipo = TC_INT;
	novoNo->chave = malloc(sizeof(char) * strlen(chave));
	strcpy(novoNo->chave, chave);
	novoNo->valor.iVal = valor;

	return addNodeNoMapa(novoNo, mapa);
}

mapa_t* addVariavelNoMapa(char *chave, variavel_t *valor, mapa_t *mapa) {
	no_t *novoNo = malloc(sizeof(no_t));
	novoNo->tipo = TC_VARIAVEL;
	novoNo->chave = malloc(sizeof(char) * (strlen(chave) + 1));
	novoNo->valor.varVal = valor;
	strcpy(novoNo->chave, chave);

	return addNodeNoMapa(novoNo, mapa);
}

void transplanteNosNoMapa(no_t* u, no_t* v, mapa_t *mapa) {
	if (u->pai == mapa->nil) {
		mapa->raiz = v;
	} else if (u == u->pai->esquerda) {
		u->pai->esquerda = v;
	} else {
		u->pai->direita = v;
	}

	v->pai = u->pai;
}

no_t* menorNo(no_t *no, mapa_t *mapa) {
	while(no->esquerda != mapa->nil) {
		no = no->esquerda;
	}
	return no;
}

void corrigirViolacoesRemocaoNoMapa(no_t *no, mapa_t *mapa) {
	while (no != mapa->raiz && no->cor != COR_PRETO) {
		if (no == no->pai->esquerda) {
			no_t *irmao = no->pai->direita;
			if (irmao->cor == COR_VERMELHO) {
				irmao->cor = COR_PRETO;
				no->pai->cor = COR_VERMELHO;
				rotacionarEsquerda(no->pai, mapa);
				irmao = no->pai->direita;
			}
			if (irmao->esquerda->cor == COR_PRETO && irmao->direita->cor == COR_PRETO) {
				irmao->cor = COR_VERMELHO;
				no = no->pai;
			} else {
				if (irmao->direita->cor == COR_PRETO) {
					irmao->esquerda->cor = COR_PRETO;
					irmao->cor = COR_VERMELHO;
					rotacionarDireita(irmao, mapa);
					irmao = no->pai->direita;
				}
				irmao->cor = no->pai->cor;
				no->pai->cor = COR_PRETO;
				irmao->direita->cor = COR_PRETO;
				rotacionarEsquerda(no->pai, mapa);
				no = mapa->raiz;
			}
		} else {
			no_t *irmao = no->pai->esquerda;
			if (irmao->cor == COR_VERMELHO) {
				irmao->cor = COR_PRETO;
				no->pai->cor = COR_VERMELHO;
				rotacionarEsquerda(no->pai, mapa);
				irmao = no->pai->esquerda;
			}
			if (irmao->direita->cor == COR_PRETO && irmao->esquerda->cor == COR_PRETO) {
				irmao->cor = COR_VERMELHO;
				no = no->pai;
			} else {
				if (irmao->esquerda->cor == COR_PRETO) {
					irmao->direita->cor = COR_PRETO;
					irmao->cor = COR_VERMELHO;
					rotacionarDireita(irmao, mapa);
					irmao = no->pai->esquerda;
				}
				irmao->cor = no->pai->cor;
				no->pai->cor = COR_PRETO;
				irmao->esquerda->cor = COR_PRETO;
				rotacionarEsquerda(no->pai, mapa);
				no = mapa->raiz;
			}
		}
	}
	no->cor = COR_PRETO;
}

void removerChaveDoMapa(char *chave, mapa_t *mapa) {
	no_t *z = buscarChaveNoMapa(chave, mapa);
	no_t *y = z;
	no_t *x;
	int cor_original_y = y->cor;

	if (z->esquerda == mapa->nil) {
		x = z->direita;
		transplanteNosNoMapa(z, z->direita, mapa);
	} else if (z->direita == mapa->nil) {
		x = z->esquerda;
		transplanteNosNoMapa(z, z->esquerda, mapa);
	} else {
		y = menorNo(z->direita, mapa);
		cor_original_y = y->cor;
		x = y->direita;
		if (y->pai == z) {
			x->pai = y;
		} else {
			transplanteNosNoMapa(y, y->direita, mapa);
			y->direita = z->direita;
			y->direita->pai = y;
		}
		transplanteNosNoMapa(z, y, mapa);
		y->esquerda = z->esquerda;
		y->esquerda->pai = y;
		y->cor = z->cor;
	}

	if (cor_original_y == COR_PRETO) {
		corrigirViolacoesRemocaoNoMapa(x, mapa);
	}
}

void printNo(no_t *no, mapa_t *mapa, GVC_t *gvc, Agraph_t *g) {
	if (no == mapa->nil) {
		return;
	}
	Agnode_t *n = agnode(g, no->chave, 1);
	if (no->cor == COR_VERMELHO) {
		agsafeset(n, "style", "filled", "");
		agsafeset(n, "fillcolor", "red", "");
	} else {
		agsafeset(n, "style", "filled", "");
		agsafeset(n, "fillcolor", "black", "");
		agsafeset(n, "fontcolor", "white", "");
	}
	if (no->pai != mapa->nil) {
		Agnode_t *p = agnode(g, no->pai->chave, 1);
		(void)agedge(g, p, n, 0, 1);
	}
	printNo(no->esquerda, mapa, gvc, g);
	printNo(no->direita, mapa, gvc, g);
}

void printMapa(mapa_t *mapa) {
	GVC_t *gvc = gvContext();
	Agraph_t *g = agopen("g", Agdirected, 0);
	printNo(mapa->raiz, mapa, gvc, g);

#ifdef NO_LAYOUT_OR_RENDERING
  // Just write the graph without layout
  agwrite(g, stdout);
#else
  // Use the directed graph layout engine
  gvLayout(gvc, g, "dot");

  // Output in .dot format
  gvRender(gvc, g, "png", stdout);

  gvFreeLayout(gvc, g);
#endif

  agclose(g);
}
