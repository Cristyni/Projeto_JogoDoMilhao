#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pergunta.h"

void inicializarBanco(BancoPerguntas *banco) {
    banco->itens = NULL;
    banco->quantidade = 0;
    banco->capacidade = 0;
}

int inserirPergunta(BancoPerguntas *banco, Pergunta nova) {
    if (banco->quantidade == banco->capacidade) {
        int novaCapacidade = (banco->capacidade == 0) ? 4 : banco->capacidade * 2;
        Pergunta *temp = (Pergunta *) realloc(banco->itens, novaCapacidade * sizeof(Pergunta));
        
        if (temp == NULL) {
            printf("Erro critico: Falha ao alocar memoria!\n");
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
int sortearPerguntasRodada(
    const BancoPerguntas *banco,
    int rodada,
    int indices[],
    int jaUsadas[]
) {
    int dificuldade;
    int quantidade;

    //definir a dificuldade e a quantidade da rodada
    if (rodada >= 1 && rodada <= 2) {
        dificuldade = 1;
        quantidade = 1;
    }
    else if (rodada >= 3 && rodada <= 4) {
        dificuldade = 2;
        quantidade = 1;
    }
    else if (rodada >= 5 && rodada <= 8) {
        dificuldade = 3;
        quantidade = 1;
    }
    else if (rodada >= 9 && rodada <= 12) {
        dificuldade = 4;
        quantidade = 1;
    }
    else if (rodada >= 13 && rodada <= 15) {
        dificuldade = 5;
        quantidade = 1;
    }
    else {
        return 0;
    }

    //vetor para guardar os indices da perguntas candidatas
    int *candidatos = malloc(banco->quantidade * sizeof(int));
    if (candidatos == NULL) {
        return 0;
    }

    int total = 0;

    for (int i = 0; i < banco->quantidade; i++) {
        if (banco->itens[i].dificuldade == dificuldade && jaUsadas[i] == 0) {
            candidatos[total] = i;
            total++;
        }
    }

    if (total < quantidade) {
        free(candidatos);
        return 0;
    }

    for (int i = total - 1; i > 0; i--) {

        int j = rand() % (i + 1);
        int auxiliar = candidatos[i];
        candidatos[i] = candidatos[j];
        candidatos[j] = auxiliar;
    }

    //escolher perguntas sorteadas e marcar seus indices como utilizados
    for (int i = 0; i < quantidade; i++) {
        indices[i] = candidatos[i];
        jaUsadas[indices[i]] = 1;
    }

    free(candidatos);

    return quantidade;
}
