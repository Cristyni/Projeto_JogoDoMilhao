/**
 * @file teste_loja.c
 * @brief Teste ISOLADO da loja do Creed e das ajudas (nao faz parte do jogo final).
 *
 * Serve para a integrante 2 testar a propria parte sem depender do resto:
 *   - tela da loja (comprar, saldo insuficiente, estoque);
 *   - uma "pergunta de teste" onde voce usa Dica, 50/50 e Trocar.
 *
 * Teclas: ESPACO = ganha +1 moeda (so neste teste).
 * Usa perguntas.dat ou perguntas.csv da pasta atual.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "pergunta.h"
#include "arquivo.h"
#include "ajudas.h"
#include "loja.h"
#include "audio.h"

#define LARGURA 1000
#define ALTURA 650

static bool botao(Rectangle r, const char *texto, Color cor, bool ativo) {
    bool sobre = ativo && CheckCollisionPointRec(GetMousePosition(), r);
    Color c = !ativo ? GRAY : (sobre ? ColorBrightness(cor, 0.25f) : cor);

    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 2, BLACK);
    DrawText(texto,
             (int)(r.x + (r.width - MeasureText(texto, 18)) / 2),
             (int)(r.y + (r.height - 18) / 2), 18, WHITE);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void textoQuebrado(const char *texto, int x, int y, int larg, int tam, Color cor) {
    char buf[512];
    char linha[512];
    char teste[512];

    strncpy(buf, texto, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    linha[0] = '\0';

    char *palavra = strtok(buf, " ");
    while (palavra != NULL) {
        if (linha[0] != '\0') {
            snprintf(teste, sizeof(teste), "%s %s", linha, palavra);
        } else {
            snprintf(teste, sizeof(teste), "%s", palavra);
        }
        if (MeasureText(teste, tam) > larg && linha[0] != '\0') {
            DrawText(linha, x, y, tam, cor);
            y += tam + 4;
            snprintf(linha, sizeof(linha), "%s", palavra);
        } else {
            snprintf(linha, sizeof(linha), "%s", teste);
        }
        palavra = strtok(NULL, " ");
    }
    if (linha[0] != '\0') {
        DrawText(linha, x, y, tam, cor);
    }
}

int main(void) {
    BancoPerguntas banco;
    inicializarBanco(&banco);

    if (!carregarBinario(&banco, "perguntas.dat") || banco.quantidade == 0) {
        liberarBanco(&banco);
        inicializarBanco(&banco);
        carregarCSV(&banco, "perguntas.csv");
    }

    if (banco.quantidade == 0) {
        printf("Nenhuma pergunta carregada. Coloque o perguntas.csv na pasta.\n");
        liberarBanco(&banco);
        return 1;
    }

    srand((unsigned int)time(NULL));

    InitWindow(LARGURA, ALTURA, "TESTE - Loja do Creed");
    SetTargetFPS(60);
    audioIniciar();
    lojaIniciar();

    InventarioAjudas inv;
    inventarioIniciar(&inv);

    int saldo = 5;                       /* comeca com moedas para testar */
    bool naLoja = true;
    int indice = rand() % banco.quantidade;
    bool ocultas[4] = { false, false, false, false };
    int dica = -1;
    char aviso[100] = "";

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            saldo++;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (naLoja) {
            /* --- a PARTIDA faria isto --- */
            AcaoLoja acao = lojaDesenhar(saldo, &inv);

            if (acao.comprou != AJUDA_NENHUMA) {
                saldo -= ajudaComprar(&inv, saldo, acao.comprou);
            }
            if (acao.continuar) {
                naLoja = false;
                inventarioNovaPergunta(&inv);
                for (int i = 0; i < 4; i++) ocultas[i] = false;
                dica = -1;
                aviso[0] = '\0';
            }

            DrawText("[ESPACO] +1 moeda (so no teste)", 20, ALTURA - 24, 16, DARKGRAY);

        } else {
            const Pergunta *p = &banco.itens[indice];
            char texto[300];

            DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 24, 32, 56, 255 });
            snprintf(texto, sizeof(texto), "PERGUNTA DE TESTE   |   MOEDAS: %02d", saldo);
            DrawText(texto, 20, 15, 22, (Color){ 240, 190, 40, 255 });

            textoQuebrado(p->enunciado, 40, 70, 920, 24, WHITE);

            for (int i = 0; i < 4; i++) {
                Rectangle r = { (float)(40 + (i % 2) * 480), (float)(200 + (i / 2) * 90), 440.0f, 75.0f };

                if (ocultas[i]) {
                    DrawRectangleRec(r, (Color){ 60, 60, 60, 255 });
                    continue;
                }
                DrawRectangleRec(r, (Color){ 52, 101, 164, 255 });
                DrawRectangleLinesEx(r, 2, BLACK);
                snprintf(texto, sizeof(texto), "%c) %s", 'A' + i, p->alternativas[i]);
                textoQuebrado(texto, (int)r.x + 12, (int)r.y + 12, 416, 18, WHITE);
            }

            if (dica >= 0) {
                snprintf(texto, sizeof(texto), "Dica do Creed: a alternativa %c NAO e a correta.", 'A' + dica);
                DrawText(texto, 40, 395, 20, (Color){ 240, 190, 40, 255 });
            }

            DrawText(aviso, 40, 430, 20, ORANGE);

            /* botoes das ajudas */
            for (int a = 0; a < QTD_AJUDAS; a++) {
                snprintf(texto, sizeof(texto), "%s (x%d)", ajudaNome((TipoAjuda)a), inv.estoque[a]);
                Rectangle r = { (float)(40 + a * 310), 480.0f, 290.0f, 50.0f };

                if (botao(r, texto, (Color){ 46, 139, 87, 255 }, true)) {
                    EfeitoAjuda e = ajudaUsar(&inv, (TipoAjuda)a, p, ocultas);

                    if (!e.valido) {
                        aviso[0] = '\0';
                        snprintf(aviso, sizeof(aviso), "%s", e.erro);
                        audioTocar(SOM_ERRO);
                    } else {
                        aviso[0] = '\0';
                        audioTocar(SOM_AJUDA);

                        for (int k = 0; k < e.qtdOcultas; k++) {
                            ocultas[e.ocultas[k]] = true;
                        }
                        if (e.dica >= 0) {
                            dica = e.dica;
                        }
                        if (e.trocarPergunta) {
                            /* a PARTIDA sortearia outra do mesmo nivel;
                               aqui so pegamos outra qualquer do banco */
                            if (banco.quantidade > 1) {
                                int novo;
                                do {
                                    novo = rand() % banco.quantidade;
                                } while (novo == indice);
                                indice = novo;
                                for (int k = 0; k < 4; k++) ocultas[k] = false;
                                dica = -1;
                                inventarioNovaPergunta(&inv);
                            } else {
                                ajudaDevolver(&inv, AJUDA_TROCA);
                                snprintf(aviso, sizeof(aviso), "Nao ha outra pergunta disponivel.");
                            }
                        }
                    }
                }
            }
            if (botao((Rectangle){ 40.0f, 570.0f, 300.0f, 50.0f }, "Voltar para a loja", (Color){ 52, 101, 164, 255 }, true)) {
                naLoja = true;
                lojaIniciar();
            }
        }

        EndDrawing();
    }

    lojaFinalizar();
    audioFinalizar();
    CloseWindow();
    liberarBanco(&banco);
    return 0;
}
