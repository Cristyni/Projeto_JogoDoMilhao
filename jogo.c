#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
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

static int premioDaRodada(int rodada) {
       const int premios[TOTAL_RODADAS] = {
        1000, 2000, 3000, 4000, 5000,
        10000, 20000, 30000, 40000, 50000,
        100000, 200000, 300000, 500000,
        1000000
    };

    if (rodada < 1 || rodada > TOTAL_RODADAS) {
        return 0;
    }

    return premios[rodada - 1];
}

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
    printf("\nRODADA %d/%d\n", rodada, TOTAL_RODADAS);
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

        while ((caractere = getchar()) != '\n'
               && caractere != EOF) {
        }

        resposta = (char)toupper((unsigned char)resposta);

        if (resposta == 'A' || resposta == 'B' ||
            resposta == 'C' || resposta == 'D' ||
            resposta == 'P') {
              return resposta;
        }

        printf("Opcao invalida! Tente novamente.\n");
    }
}

   void iniciarJogo(BancoPerguntas *banco) {
    int acertos = 0;
    int premioAcumulado = 0;
    int premioFinal = 0;
    int ultimaEtapa = 0;

    if (banco == NULL || banco->itens == NULL || banco->quantidade == 0) {
        printf("\nNenhuma pergunta disponivel!\n");
        return;
    }

    // Marca as perguntas ja utilizadas
    int *jaUsadas = calloc(banco->quantidade, sizeof(int));

    if (jaUsadas == NULL) {
        printf("\nErro ao alocar memoria!\n");
        return;
    }

    // Inicializa o sorteio uma unica vez
    static int sorteioInicializado = 0;

    if (!sorteioInicializado) {
        srand((unsigned int)time(NULL));
        sorteioInicializado = 1;
    }

    printf("\n THE OFFICE - DUNDIES \n");

    for (int rodada = 1; rodada <= TOTAL_RODADAS; rodada++) {

        int indices[1];

        int quantidade = sortearPerguntasRodada(
            banco, rodada, indices, jaUsadas
        );

        if (quantidade != 1) {
            printf("\nPerguntas insuficientes para continuar.\n");

            premioFinal = premioAcumulado;
            break;
        }

        const Pergunta *perguntaAtual =
            &banco->itens[indices[0]];

        const char *alternativas[4];

        for (int i = 0; i < 4; i++) {
            alternativas[i] = perguntaAtual->alternativas[i];
        }


        int premioSeErrar;

        if (rodada == TOTAL_RODADAS) {
            premioSeErrar = 0;
        } else {
            premioSeErrar = premioAcumulado / 2;

            // Respeita os marcos de seguranca conquistados
            if (ultimaEtapa >= 10 && premioSeErrar < 50000) {
                premioSeErrar = 50000;
            } else if (ultimaEtapa >= 5 && premioSeErrar < 5000) {
                premioSeErrar = 5000;
            }
}

        mostrarRodada(rodada, acertos);

        printf("\nVALORES DA PERGUNTA:\n");
        printf("Acertar: R$ %d\n", premioDaRodada(rodada));
        printf("Parar:   R$ %d\n", premioAcumulado);
        printf("Errar:   R$ %d\n", premioSeErrar);

        mostrarPergunta(
            perguntaAtual->enunciado,
            alternativas
        );

        char resposta = lerResposta();

        if (resposta == 'P') {
            premioFinal = premioAcumulado;

            printf("\nVoce decidiu parar!\n");
            printf("Voce leva R$ %d!\n", premioFinal);
            break;
        }

        char correta = (char)toupper(
            (unsigned char)perguntaAtual->letraCorreta
        );

        if (resposta != correta) {
            premioFinal = premioSeErrar;

            printf("\nRESPOSTA ERRADA!\n");
            printf("Resposta correta: %c\n", correta);

            if (rodada == TOTAL_RODADAS) {
                printf("Voce arriscou tudo na pergunta do milhao!\n");
                printf("Premio final: R$ 0\n");
            } else {
                printf("Premio final: R$ %d\n", premioFinal);
            }

            break;
        }

        acertos++;
        premioAcumulado = premioDaRodada(rodada);

        printf("\nRESPOSTA CORRETA!\n");
        printf("Premio conquistado: R$ %d\n",
               premioAcumulado);

        if (rodada == 5 || rodada == 10) {
            ultimaEtapa = rodada;

            printf("\nMarco da rodada %d alcancado!\n",
                   rodada);
        }

        if (rodada == TOTAL_RODADAS) {
            premioFinal = premioAcumulado;

            printf("\nPARABENS! VOCE VENCEU!\n");
            printf("VOCE GANHOU R$ 1000000!\n");
        }
    }

    printf("\n     RESULTADO FINAL \n");
    printf("Perguntas acertadas: %d\n", acertos);
    printf("Ultimo marco: %d\n", ultimaEtapa);
    printf("Premio final: R$ %d\n", premioFinal);

    free(jaUsadas);
}
