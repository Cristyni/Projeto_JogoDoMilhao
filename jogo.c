#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "interface.h"
#include "arquivo.h"
#include "jogo.h"


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

const char *obterDificuldade(int rodada) {
    if (rodada <= 2) {
        return "Muito facil";
    } else if (rodada <= 4) {
        return "Facil";
    } else if (rodada <= 8) {
        return "Medio";
    } else if (rodada <= 12) {
        return "Dificil";
    } else {
        return "Muito dificil";
    }
}

void mostrarRodada(int rodada, int pontuacao) {
    printf("RODADA %d/%d\n", rodada, TOTAL_RODADAS);
    printf("Dificuldade: %s\n", obterDificuldade(rodada));
    printf("Pontuacao: %d\n", pontuacao);
}

void mostrarPergunta(const char *enunciado, const char *alternativas[4]) {

       printf("\n%s\n\n", enunciado);

       for (int i = 0; i < 4; i++) {
              printf("%c) %s\n", 'A' + i, alternativas[i]);
       }
}

char lerResposta(void) {
    char resposta;
    int caractere;

    while (1) {
        printf("\nA, B, C, D ou P para parar: ");

        if (scanf(" %c", &resposta) != 1) {
            return 'P';
        }

        // Limpa o restante da linha
        while ((caractere = getchar()) != '\n'
               && caractere != EOF) {
        }

        resposta = toupper((unsigned char)resposta); //muda minusculas pra maiusculas

        if (resposta == 'A' || resposta == 'B' ||
            resposta == 'C' || resposta == 'D' ||
            resposta == 'P') {
              return resposta;
        }

        printf("Opcao invalida! Tente novamente\n");
    }
}


void iniciarJogo(void) {
    int rodada;
    int acertos = 0;
    int marcoSeguranca = 0;
    char resposta;

    const char *perguntaTeste =
        "Quem e o gerente regional da Dunder Mifflin?";

    const char *alternativasTeste[4] = {
        "Jim Halpert",
        "Michael Scott",
        "Dwight Schrute",
        "Kevin Malone"
    };

    printf("\n    THE OFFICE \n");
    printf("       DUNDIES\n");

    for (rodada = 1; rodada <= TOTAL_RODADAS; rodada++) {

        mostrarRodada(rodada, acertos);
        mostrarPergunta(perguntaTeste, alternativasTeste);

        resposta = lerResposta();

        if (resposta == 'P') {
            printf("\nVoce desistiu da partida!\n");
            break;
        }

        if (resposta != 'B') {
            printf("\nResposta errada!\n");
            printf("A resposta correta era B.\n");
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

    printf("\n   RESULTADO FINAL \n");
    printf("Perguntas acertadas: %d\n", acertos);
    printf("Ultimo marco: %d\n", marcoSeguranca);
    printf("Fim da partida!\n");
}
