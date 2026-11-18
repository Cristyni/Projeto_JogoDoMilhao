#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pergunta.h"

void inicializarBanco(BancoPerguntas *banco) {
    banco->capacidade = 4;
    banco->quantidade = 0;

    banco->itens = (Pergunta *) malloc(banco->capacidade * sizeof(Pergunta));

    if (banco->itens == NULL) {
        printf("Erro critico: Falha ao alocar memoria com malloc!\n");
        banco->capacidade = 0;
    }
}

int inserirPergunta(BancoPerguntas *banco, Pergunta nova) {
    if (banco->itens == NULL) {
        banco->capacidade = 4;
        banco->itens = (Pergunta *) malloc(banco->capacidade * sizeof(Pergunta));
        if (banco->itens == NULL) {
            printf("Erro critico: Falha ao alocar memoria com malloc!\n");
            return 0;
        }
    }

    if (banco->quantidade == banco->capacidade) {
        int novaCapacidade = banco->capacidade * 2;
        Pergunta *temp = (Pergunta *) realloc(banco->itens, novaCapacidade * sizeof(Pergunta));

        if (temp == NULL) {
            printf("Erro critico: Falha ao realocar memoria!\n");
            return 0;
        }

        banco->itens = temp;
        banco->capacidade = novaCapacidade;
    }

    banco->itens[banco->quantidade] = nova;
    banco->quantidade++;
    return 1;
}

void listarPerguntas(const BancoPerguntas *banco) {
    if (banco->quantidade == 0) {
        printf("Nenhuma pergunta cadastrada no momento.\n");
        return;
    }

    for (int i = 0; i < banco->quantidade; i++) {
        printf("\n--- Pergunta [%d] (Dificuldade: %d) ---\n", i, banco->itens[i].dificuldade);
        printf("Enunciado: %s\n", banco->itens[i].enunciado);
        printf("A) %s\n", banco->itens[i].alternativas[0]);
        printf("B) %s\n", banco->itens[i].alternativas[1]);
        printf("C) %s\n", banco->itens[i].alternativas[2]);
        printf("D) %s\n", banco->itens[i].alternativas[3]);
        printf("Correta: %c\n", banco->itens[i].letraCorreta);
    }
}

int pesquisarPerguntaPorTexto(const BancoPerguntas *banco, const char *termo) {
    for (int i = 0; i < banco->quantidade; i++) {
        if (strstr(banco->itens[i].enunciado, termo) != NULL) {
            return i;
        }
    }
    return -1;
}

int alterarPergunta(BancoPerguntas *banco, int indice, Pergunta atualizada) {
    if (indice < 0 || indice >= banco->quantidade) {
        return 0;
    }
    banco->itens[indice] = atualizada;
    return 1;
}

int excluirPergunta(BancoPerguntas *banco, int indice) {
    if (indice < 0 || indice >= banco->quantidade) {
        return 0;
    }

    for (int i = indice; i < banco->quantidade - 1; i++) {
        banco->itens[i] = banco->itens[i + 1];
    }

    banco->quantidade--;

    if (banco->quantidade > 0 && banco->quantidade <= banco->capacidade / 4 && banco->capacidade > 4) {
        int novaCapacidade = banco->capacidade / 2;
        Pergunta *temp = (Pergunta *) realloc(banco->itens, novaCapacidade * sizeof(Pergunta));
        if (temp != NULL) {
            banco->itens = temp;
            banco->capacidade = novaCapacidade;
        }
    } else if (banco->quantidade == 0) {
        liberarBanco(banco);
    }

    return 1;
}

void liberarBanco(BancoPerguntas *banco) {
    if (banco->itens != NULL) {
        free(banco->itens);
        banco->itens = NULL;
    }
    banco->quantidade = 0;
    banco->capacidade = 0;
}
