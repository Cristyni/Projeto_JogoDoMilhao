#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "interface.h"
#include "arquivo.h"
#include "jogo.h"
#include "pergunta.h"


/**
 * @file jogo.c
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */



#define TOTAL_RODADAS 15
// Verifica se o indice ja apareceu nesta partida.
static int indiceJaUsado(int indice, const int usados[], int qtdUsados) {
    for (int i = 0; i < qtdUsados; i++) {
        if (usados[i] == indice) return 1;
    }
    return 0;
}

// Sorteia uma pergunta da dificuldade desejada, sem repetir indices.
// Retorna -1 quando nao ha perguntas disponiveis nesse nivel.
static int sortearIndicePergunta(const BancoPerguntas *banco, int dificuldade,
                                const int usados[], int qtdUsados) {
    int disponiveis = 0;

    for (int i = 0; i < banco->quantidade; i++) {
        if (banco->itens[i].dificuldade == dificuldade &&
            !indiceJaUsado(i, usados, qtdUsados)) {
            disponiveis++;
        }
    }

    if (disponiveis == 0) return -1;

    int sorteado = rand() % disponiveis;

    for (int i = 0; i < banco->quantidade; i++) {
        if (banco->itens[i].dificuldade == dificuldade &&
            !indiceJaUsado(i, usados, qtdUsados)) {
            if (sorteado == 0) return i;
            sorteado--;
        }
    }
    return -1;
}

void iniciarJogo(BancoPerguntas *banco) {
    int acertos = 0;
    int marcoSeguranca = 0;
    int usados[TOTAL_RODADAS];
    int qtdUsados = 0;
    static int sorteioInicializado = 0;

    if (banco == NULL || banco->itens == NULL || banco->quantidade == 0) {
        printf("\nNenhuma pergunta disponivel para jogar!\n");
        return;
    }

    if (!sorteioInicializado) {
        srand((unsigned int)time(NULL));
        sorteioInicializado = 1;
    }

    printf("\n    THE OFFICE\n");
    printf("       DUNDIES\n");

    for (int rodada = 1; rodada <= TOTAL_RODADAS; rodada++) {
        int dificuldade = nivelDaRodada(rodada);
        int indice = sortearIndicePergunta(banco, dificuldade, usados, qtdUsados);

        if (indice == -1) {
            printf("\nNao ha perguntas disponiveis do nivel %d (%s).\n",
                   dificuldade, obterDificuldade(rodada));
            printf("Nao e possivel continuar a partida.\n");
            break;
        }

        usados[qtdUsados] = indice;
        qtdUsados++;

        const Pergunta *perguntaAtual = &banco->itens[indice];
        const char *alternativas[4];
        for (int i = 0; i < 4; i++) {
            alternativas[i] = perguntaAtual->alternativas[i];
        }

        mostrarRodada(rodada, acertos);
        mostrarPergunta(perguntaAtual->enunciado, alternativas);

        char resposta = lerResposta();

        if (resposta == 'P') {
            printf("\nVoce desistiu da partida!\n");
            break;
        }

        if (resposta != perguntaAtual->letraCorreta) {
            printf("\nResposta errada!\n");
            printf("A resposta correta era %c.\n", perguntaAtual->letraCorreta);
            break;
        }

        acertos++;
        printf("\nResposta correta!\n");

        if (rodada == 5 || rodada == 10) {
            marcoSeguranca = rodada;
            printf("Marco de seguranca alcancado!\n");
        }

        if (rodada == TOTAL_RODADAS) {
            printf("\nPARABENS! Voce venceu!\n");
        }
    }

    printf("\n   RESULTADO FINAL\n");
    printf("Perguntas acertadas: %d\n", acertos);
    printf("Ultimo marco: %d\n", marcoSeguranca);
    printf("Fim da partida!\n");
}