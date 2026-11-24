/**
 * @file loja.c
 * @brief Tela estatica do banheiro do restaurante: Creed vende as ajudas.
 *
 * Imagens OPCIONAIS (se nao existirem, aparece um desenho simples no lugar):
 *   assets/visuais/creed.png
 *   assets/icones/caneca.png   assets/icones/gelatina.png   assets/icones/papel.png
 */

#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "loja.h"
#include "audio.h"

#define TEMPO_MENSAGEM 2.5      /* segundos que a mensagem de compra fica na tela */

/* Cores */
static const Color COR_FUNDO    = { 200, 215, 220, 255 };
static const Color COR_AZULEJO  = { 170, 190, 198, 255 };
static const Color COR_PAINEL   = { 40, 52, 90, 255 };
static const Color COR_DOURADO  = { 240, 190, 40, 255 };
static const Color COR_VERDE    = { 46, 139, 87, 255 };
static const Color COR_AZUL     = { 52, 101, 164, 255 };
static const Color COR_CINZA    = { 120, 120, 120, 255 };
static const Color COR_ERRO     = { 178, 34, 34, 255 };

/* Caminhos das imagens opcionais */
static const char *CAMINHO_CREED = "assets/visuais/creed.png";
static const char *CAMINHOS_ICONES[QTD_AJUDAS] = {
    "assets/icones/caneca.png",
    "assets/icones/gelatina.png",
    "assets/icones/papel.png"
};

/* Estado interno da loja (so este arquivo enxerga) */
static Texture2D texCreed;                  /* id == 0 -> nao carregada */
static Texture2D texIcone[QTD_AJUDAS];
static char      mensagem[96];
static bool      mensagemOk;                /* true = confirmacao, false = erro */
static double    mensagemAte;               /* ate quando mostrar a mensagem    */

/* ------------------------------------------------------------------ */
/* Auxiliares                                                          */
/* ------------------------------------------------------------------ */
static Texture2D carregarSeExistir(const char *caminho) {
    Texture2D tex = { 0 };

    if (FileExists(caminho)) {
        tex = LoadTexture(caminho);
    }
    return tex;
}

static void desenharImagem(Texture2D tex, Rectangle destino) {
    Rectangle origem = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Vector2 ponto = { 0.0f, 0.0f };

    DrawTexturePro(tex, origem, destino, ponto, 0.0f, WHITE);
}

/* Texto que quebra em varias linhas para caber na largura dada */
static void textoQuebrado(const char *texto, int x, int y, int larg, int tam, Color cor) {
    char buf[256];
    char linha[256];
    char teste[256];

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

/* Desenha um botao e devolve true se foi clicado neste frame */
static bool botao(Rectangle r, const char *texto, Color cor) {
    bool sobre = CheckCollisionPointRec(GetMousePosition(), r);
    Color c = sobre ? ColorBrightness(cor, 0.25f) : cor;

    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 2, BLACK);

    int larguraTexto = MeasureText(texto, 20);
    DrawText(texto,
             (int)(r.x + (r.width - larguraTexto) / 2),
             (int)(r.y + (r.height - 20) / 2),
             20, WHITE);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void mostrarMensagem(const char *texto, bool ok) {
    snprintf(mensagem, sizeof(mensagem), "%s", texto);
    mensagemOk = ok;
    mensagemAte = GetTime() + TEMPO_MENSAGEM;
}

/* ------------------------------------------------------------------ */
/* Interface publica                                                   */
/* ------------------------------------------------------------------ */
void lojaIniciar(void) {
    if (texCreed.id == 0) {
        texCreed = carregarSeExistir(CAMINHO_CREED);
    }
    for (int i = 0; i < QTD_AJUDAS; i++) {
        if (texIcone[i].id == 0) {
            texIcone[i] = carregarSeExistir(CAMINHOS_ICONES[i]);
        }
    }

    mensagem[0] = '\0';
    mensagemAte = 0.0;
}

void lojaFinalizar(void) {
    if (texCreed.id != 0) {
        UnloadTexture(texCreed);
        memset(&texCreed, 0, sizeof(texCreed));
    }
    for (int i = 0; i < QTD_AJUDAS; i++) {
        if (texIcone[i].id != 0) {
            UnloadTexture(texIcone[i]);
            memset(&texIcone[i], 0, sizeof(texIcone[i]));
        }
    }
}

AcaoLoja lojaDesenhar(int saldo, const InventarioAjudas *inv) {
    AcaoLoja acao;
    char texto[160];

    acao.comprou = AJUDA_NENHUMA;
    acao.continuar = false;

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    /* ---- fundo: azulejos do banheiro ---- */
    DrawRectangle(0, 0, w, h, COR_FUNDO);
    for (int x = 0; x < w; x += 60) {
        DrawLine(x, 0, x, h, COR_AZULEJO);
    }
    for (int y = 0; y < h; y += 60) {
        DrawLine(0, y, w, y, COR_AZULEJO);
    }

    /* ---- barra superior: titulo e saldo ---- */
    DrawRectangle(0, 0, w, 60, COR_PAINEL);
    DrawText("BANHEIRO DO RESTAURANTE - LOJA DO CREED", 20, 18, 24, COR_DOURADO);

    snprintf(texto, sizeof(texto), "MOEDAS: %02d", saldo);
    DrawText(texto, w - MeasureText(texto, 24) - 20, 18, 24, WHITE);

    /* ---- Creed (imagem ou desenho simples) ---- */
    Rectangle balao = { 30.0f, 80.0f, 290.0f, 70.0f };
    DrawRectangleRec(balao, WHITE);
    DrawRectangleLinesEx(balao, 2, BLACK);
    DrawText("Psiu... quer uns poderes?", 42, 94, 18, BLACK);
    DrawText("Tenho um preco bom.", 42, 120, 18, BLACK);

    Rectangle areaCreed = { 30.0f, 165.0f, 290.0f, (float)(h - 165 - 120) };
    if (texCreed.id != 0) {
        desenharImagem(texCreed, areaCreed);
    } else {
        DrawRectangleRec(areaCreed, COR_PAINEL);
        DrawRectangleLinesEx(areaCreed, 2, COR_DOURADO);
        DrawText("CREED", (int)areaCreed.x + 90, (int)areaCreed.y + 20, 30, COR_DOURADO);
        textoQuebrado("(coloque a imagem em assets/visuais/creed.png)",
                      (int)areaCreed.x + 15, (int)areaCreed.y + 70,
                      (int)areaCreed.width - 30, 16, LIGHTGRAY);
    }

    /* ---- cartoes das tres ajudas ---- */
    int x0 = 350;
    int larguraCartao = w - x0 - 30;
    int topo = 80;
    int fundo = h - 120;
    int espaco = 12;
    int alturaCartao = (fundo - topo - 2 * espaco) / QTD_AJUDAS;

    for (int a = 0; a < QTD_AJUDAS; a++) {
        TipoAjuda tipo = (TipoAjuda)a;
        int y = topo + a * (alturaCartao + espaco);
        int preco = ajudaPreco(tipo);
        int tem = (inv != NULL) ? inv->estoque[a] : 0;

        DrawRectangle(x0, y, larguraCartao, alturaCartao, COR_PAINEL);
        DrawRectangleLines(x0, y, larguraCartao, alturaCartao, COR_DOURADO);

        /* icone */
        int lado = alturaCartao - 20;
        Rectangle caixaIcone = { (float)(x0 + 10), (float)(y + 10), (float)lado, (float)lado };

        if (texIcone[a].id != 0) {
            desenharImagem(texIcone[a], caixaIcone);
        } else {
            char inicial[2] = { ajudaNome(tipo)[0], '\0' };
            DrawRectangleRec(caixaIcone, COR_AZUL);
            DrawText(inicial,
                     (int)caixaIcone.x + (lado - MeasureText(inicial, 48)) / 2,
                     (int)caixaIcone.y + (lado - 48) / 2,
                     48, WHITE);
        }

        /* textos */
        int xTexto = x0 + lado + 25;
        int larguraTexto = larguraCartao - lado - 25 - 175;

        DrawText(ajudaNome(tipo), xTexto, y + 12, 22, WHITE);
        textoQuebrado(ajudaDescricao(tipo), xTexto, y + 44, larguraTexto, 16, LIGHTGRAY);

        snprintf(texto, sizeof(texto), "Preco: %d moedas | Voce tem: %d", preco, tem);
        DrawText(texto, xTexto, y + alturaCartao - 32, 18, COR_DOURADO);

        /* botao comprar (cinza se nao ha saldo, mas ainda clicavel para avisar) */
        Rectangle rb = { (float)(x0 + larguraCartao - 165),
                         (float)(y + (alturaCartao - 50) / 2), 150.0f, 50.0f };
        Color corBotao = (saldo >= preco) ? COR_VERDE : COR_CINZA;

        if (botao(rb, "Comprar", corBotao)) {
            if (saldo >= preco) {
                acao.comprou = tipo;
                snprintf(texto, sizeof(texto), "Compra confirmada: %s!", ajudaNome(tipo));
                mostrarMensagem(texto, true);
                audioTocar(SOM_COMPRA);
            } else {
                mostrarMensagem("Moedas insuficientes!", false);
                audioTocar(SOM_ERRO);
            }
        }
    }

    /* ---- mensagem de compra (confirmada ou saldo insuficiente) ---- */
    if (mensagem[0] != '\0' && GetTime() < mensagemAte) {
        DrawText(mensagem,
                 (w - MeasureText(mensagem, 22)) / 2,
                 h - 108, 22,
                 mensagemOk ? COR_VERDE : COR_ERRO);
    }

    /* ---- continuar ---- */
    Rectangle rc = { (float)(w / 2 - 170), (float)(h - 70), 340.0f, 50.0f };
    if (botao(rc, "Seguir para a pergunta", COR_AZUL)) {
        acao.continuar = true;
    }

    
    return acao;
}
