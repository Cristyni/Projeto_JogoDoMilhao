#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "interface.h"

void iniciarPartida(BancoPerguntas *banco, int modoCompetitivo) {
    if (banco->quantidade < 15) {
        printf("\n[Erro] O banco precisa ter pelo menos 15 perguntas cadastradas para iniciar o jogo!\n");
        return;
    }

    int pontos = 0;
    int moedas = 0;
    int grampeadorGelatina = 1; // Poderes disponíveis na partida
    int canecaDica = 1;
    int folhaPapel = 1;

    printf("\n--- BEM-VINDO AOS DUNDIES! ---\n");
    printf("Michael Scott esta apresentando o evento e Dwight esta no teclado!\n");

    // Exemplo simplificado do fluxo de rodadas por dificuldade
    for (int nivel = 1; nivel <= 5; nivel++) {
        printf("\n=== NIVEL DE DIFICULDADE: %d ===\n", nivel);
        
        // Busca uma pergunta aleatória do nível correspondente
        int encontrada = 0;
        for (int i = 0; i < banco->quantidade; i++) {
            if (banco->itens[i].dificuldade == nivel) {
                Pergunta p = banco->itens[i];
                printf("\nEnunciado: %s\n", p.enunciado);
                printf("A) %s\n", p.alternativas[0]);
                printf("B) %s\n", p.alternativas[1]);
                printf("C) %s\n", p.alternativas[2]);
                printf("D) %s\n", p.alternativas[3]);

                if (moedas > 0) {
                    printf("\n[Moedas: %d] Deseja ir ao banheiro com o Creed comprar poderes? (1-Sim / 0-Nao): ", moedas);
                    int opcaoCreed;
                    scanf("%d", &opcaoCreed);
                    limparBuffer();
                    if (opcaoCreed == 1 && moedas >= 5) {
                        printf("Creed vendeu um poder secreto!\n");
                        moedas -= 5;
                    }
                }

                printf("Sua resposta (A, B, C, D): ");
                char resposta;
                scanf(" %c", &resposta);
                limparBuffer();

                if (resposta >= 'a' && resposta <= 'z') resposta -= 32;

                if (resposta == p.letraCorreta) {
                    printf("Acertou! Sons de teclado do Dwight comemoram!\n");
                    pontos += nivel * 100;
                    moedas += 2;
                } else {
                    printf("Errou! A resposta correta era: %c\n", p.letraCorreta);
                    printf("Fim de jogo para voce nos Dundies!\n");
                    return;
                }
                encontrada = 1;
                break; // Avança para o próximo nível no exemplo
            }
        }
        if (!encontrada) {
            printf("Nao ha perguntas suficientes para o nivel %d.\n", nivel);
        }
    }

    printf("\nParabens! Voce concluiu o jogo com %d pontos e ganhou um Dundie!\n", pontos);
}