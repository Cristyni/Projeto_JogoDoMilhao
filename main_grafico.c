/**
 * @file main_grafico.c
 * @brief Interface grafica (raylib) do The Office - Show do Milhao.
 *
 * Reaproveita o back-end ja pronto: pergunta.c/.h e arquivo.c/.h.
 * NAO usa interface.c nem jogo.c (esses sao da versao de console).
 *
 * Ideia central da raylib: UM unico loop. A cada frame (60x por segundo)
 * o programa desenha a tela atual e, no mesmo lugar, checa os cliques.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "arquivo.h"
#include "raylib.h"
#include "pergunta.h"

/*Constantes de preços e regras*/
#define LARGURA 1000
#define ALTURA 650
#define TOTAL_RODADAS 15
#define MOEDAS_POR_ACERTO 1

enum { AJUDA_DICA, AJUDA_5050, AJUDA_TROCA, QTD_AJUDAS };

static const int PRECOS[QTD_AJUDAS] = { 2, 3, 4 };

static const char *NOMES_AJUDAS[QTD_AJUDAS] = {
    "Caneca de café",
    "Grampeador na gelatina",
};

static const char *NOMES_CURTOS[QTD_AJUDAS] = { "Dica", "50/50", "Trocar" };
static const int PREMIOS[TOTAL_RODADAS] = {
    1000, 2000, 3000, 4000, 5000,
    10000, 20000, 30000, 40000, 50000,
    100000, 200000, 300000, 500000,
    1000000

};

/* Tempo de resposta da CPU por dificuldade: 0 facil, 1 medio, 2 dificil */

static const int TEMPO_MIN[3] = { 10, 6, 2 };
static const int TEMPO_MAX[3] = { 15, 10, 5 };
static const char *NOMES_NIVEL[3] = { "Facil", "Medio", "Dificil" };

/* Cores */
static const Color FUNDO   = { 24, 32, 56, 255 };
static const Color PAINEL  = { 40, 52, 90, 255 };
static const Color AZUL    = { 52, 101, 164, 255 };
static const Color VERDE   = { 46, 139, 87, 255 };
static const Color VERMELHO= { 178, 34, 34, 255 };
static const Color DOURADO = { 240, 190, 40, 255 };
static const Color COR_TITULO_PRINCIPAL = { 240, 190, 40, 255 };
static const Color COR_SUBTITULO = { 255, 255, 255, 255 }; 
static const Color COR_TEXTO_PAM = { 255, 255, 255, 255 }; 
static const Color COR_BOTAO_MENU = { 138, 154, 91, 255 };  
static const Color COR_BOTAO_SAIR = { 183, 65, 14, 255 };   
static const Color COR_TEXTO_BOTAO = { 255, 255, 255, 255 }; 
/* Estado do popup do modo Solo */
static bool mostrarPopupResposta = false;
static bool popupAcertou = false;
static int popupPontos = 0;
static bool mostrarPopupDesistencia = false;

static Texture2D texFundoMenu = { 0 };
static Texture2D texPamMenu   = { 0 };
static Texture2D texPamVitoria = { 0 };
static Texture2D texAngela = { 0 };
static Texture2D texMichael = { 0 };
static Texture2D texFundoBanheiro = { 0 };
static Texture2D texCreed         = { 0 };
static Texture2D texPoderes[3]    = { { 0 }, { 0 }, { 0 } };


typedef enum {
    TELA_MENU,TELA_DIFICULDADE, TELA_PERGUNTA,TELA_FEEDBACK,TELA_LOJA, TELA_RESULTADO
} Tela;

typedef struct {

    BancoPerguntas *banco;
    Tela tela;
    bool sair;
    bool modoCPU;
    int nivelCPU;              /* 0, 1 ou 2 */
    int rodada;                /* 1..15 */
    int moedas;
    int acertos;
    int pontosPam, pontosCPU;
    int premioAcumulado, premioFinal;
    bool fimDeJogo;
    int *jaUsadas;             /* perguntas ja sorteadas nesta partida */
    int indice;                /* indice da pergunta atual no banco */
    int estoque[QTD_AJUDAS];          /* ajudas compradas e ainda nao usadas */
    bool usadaNestaPergunta[QTD_AJUDAS];
    bool oculta[4];            /* alternativas removidas pelo 50/50 */
    int dica;                  /* alternativa revelada como errada (-1 = nenhuma) */
    bool pamRespondeu;
    bool cpuRespondeu;
    char respostaCPU;
    int tempoCPU;
    double inicioRodada;
    char mensagem[200];

} Jogo;

/* ------------------------------------------------------------------ */
/* FUNCOES AUXILIARES DE DESENHO                                       */
/* ------------------------------------------------------------------ */
/* Calcula a altura exata (em pixels) que um texto quebrado vai ocupar */

static int calcularAlturaTexto(const char *texto, int larg, int tam) {

    char buf[512];
    char linha[512];
    char teste[512];
    int linhas = 1;

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
            linhas++;
            snprintf(linha, sizeof(linha), "%s", palavra);
        } else {
            snprintf(linha, sizeof(linha), "%s", teste);
        }
        palavra = strtok(NULL, " ");
    }
    return linhas * tam + (linhas - 1) * 6;
}

/* Escreve texto quebrando em varias linhas para caber na largura dada */
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
            y += tam + 6;
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

    static bool botaoArredondado(Rectangle r, const char *texto, Color cor, bool ativo) {
    bool sobre = ativo && CheckCollisionPointRec(GetMousePosition(), r);
    Color c = !ativo ? GRAY : (sobre ? ColorBrightness(cor, 0.25f) : cor);
    /* 0.4f controla o nivel de arredondamento (0.0 = quadrado, 1.0 = pilula) */
    DrawRectangleRounded(r, 0.4f, 12, c);
    DrawRectangleRoundedLinesEx(r, 0.4f, 12, 2, BLACK);
    int larguraTexto = MeasureText(texto, 20);

    DrawText(texto,
             (int)(r.x + (r.width - larguraTexto) / 2),
             (int)(r.y + (r.height - 20) / 2),20, COR_TEXTO_BOTAO);
    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
static bool botao(Rectangle r, const char *texto, Color cor, bool ativo) {
    bool sobre = ativo && CheckCollisionPointRec(GetMousePosition(), r);
    Color c = !ativo ? GRAY : (sobre ? ColorBrightness(cor, 0.25f) : cor);
    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 2, BLACK);
    int larguraTexto = MeasureText(texto, 20);
    DrawText(texto,
             (int)(r.x + (r.width - larguraTexto) / 2),
             (int)(r.y + (r.height - 20) / 2),
             20, WHITE);
    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static bool botaoAlternativa(Rectangle r, const char *texto, bool ativo) {

    bool sobre = ativo && CheckCollisionPointRec(GetMousePosition(), r);
    Color c = !ativo ? GRAY : (sobre ? ColorBrightness(AZUL, 0.25f) : AZUL);
    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 2, BLACK);
    textoQuebrado(texto, (int)r.x + 12, (int)r.y + 12, (int)r.width - 24, 18, WHITE);
    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void centralizado(const char *texto, int y, int tam, Color cor) {
    DrawText(texto, (LARGURA - MeasureText(texto, tam)) / 2, y, tam, cor);
}

/* ------------------------------------------------------------------ */
/* LOGICA DA PARTIDA                                                   */
/* ------------------------------------------------------------------ */

static char letraCorreta(const Jogo *j) {
    return (char)toupper((unsigned char)j->banco->itens[j->indice].letraCorreta);

}

static void limparEfeitosDaPergunta(Jogo *j) {
    for (int i = 0; i < 4; i++) j->oculta[i] = false;
    j->dica = -1;

}

/* Sorteia a pergunta da rodada e (no modo PC) a resposta e o tempo da CPU */
static void iniciarRodada(Jogo *j) {

    int idx[1];
    if (sortearPerguntasRodada(j->banco, j->rodada, idx, j->jaUsadas) != 1) {
        snprintf(j->mensagem, sizeof(j->mensagem), "Perguntas insuficientes para continuar.");
        j->premioFinal = j->premioAcumulado;
        j->fimDeJogo = true;
        j->tela = TELA_RESULTADO;

        return;
    }

    j->indice = idx[0];
    limparEfeitosDaPergunta(j);
    for (int i = 0; i < QTD_AJUDAS; i++) j->usadaNestaPergunta[i] = false;
    j->pamRespondeu = false;
    j->cpuRespondeu = false;
    if (j->modoCPU) {
        /* sorteado UMA vez por pergunta, sem olhar o gabarito */

        int sorteio = rand() % 24;
        int mn = TEMPO_MIN[j->nivelCPU];
        int mx = TEMPO_MAX[j->nivelCPU];
        j->respostaCPU = (char)('A' + sorteio / 6);
        j->tempoCPU = mn + rand() % (mx - mn + 1);

    }

    j->inicioRodada = GetTime();
    j->tela = TELA_PERGUNTA;

}

static void novaPartida(Jogo *j, bool modoCPU, int nivelCPU) {

    free(j->jaUsadas);
    j->jaUsadas = calloc(j->banco->quantidade, sizeof(int));
    j->modoCPU = modoCPU;
    j->nivelCPU = nivelCPU;
    j->rodada = 1;
    j->moedas = 0;
    j->acertos = 0;
    j->pontosPam = 0;
    j->pontosCPU = 0;
    j->premioAcumulado = 0;
    j->premioFinal = 0;
    j->fimDeJogo = false;

    mostrarPopupResposta = false;
    mostrarPopupDesistencia = false;
    popupAcertou = false;
    popupPontos = 0;

    for (int i = 0; i < QTD_AJUDAS; i++) j->estoque[i] = 0;
    j->mensagem[0] = '\0';
    if (j->jaUsadas == NULL) {
        j->tela = TELA_MENU;
        return;
    }

    iniciarRodada(j);

}

/* Encerra a rodada atual e vai para a tela de feedback */

/* Encerra a rodada atual e abre o popup de resultado */
static void finalizarRodada(Jogo *j) {
    j->cpuRespondeu = true;
    if (j->rodada >= TOTAL_RODADAS) {
        j->fimDeJogo = true;
    }
    mostrarPopupResposta = true;
}

/* Pam clicou em uma alternativa */
/* Pam clicou em uma alternativa */
static void responder(Jogo *j, char letra) {
    char certa = letraCorreta(j);
    j->pamRespondeu = true;

    if (letra == certa) {
        j->moedas += MOEDAS_POR_ACERTO;
        if (j->modoCPU) {
            j->pontosPam++;
            popupAcertou = true;
            snprintf(j->mensagem, sizeof(j->mensagem),
                     "Pam acertou primeiro! +1 ponto e +%d moeda.", MOEDAS_POR_ACERTO);
            finalizarRodada(j);
        } else {
            j->acertos++;
            j->premioAcumulado = PREMIOS[j->rodada - 1];
            if (j->rodada == TOTAL_RODADAS) {
                j->premioFinal = j->premioAcumulado;
            }
            mostrarPopupResposta = true;
            popupAcertou = true;
            popupPontos = PREMIOS[j->rodada - 1];
        }
    } else {
        if (j->modoCPU) {
            if (j->cpuRespondeu) {
                popupAcertou = false;
                snprintf(j->mensagem, sizeof(j->mensagem), "Os dois erraram!");
                finalizarRodada(j);
            }
        } else {
            j->premioFinal = (j->rodada == TOTAL_RODADAS) ? 0 : j->premioAcumulado / 2;
            j->fimDeJogo = true;
            mostrarPopupResposta = true;
            popupAcertou = false;
            popupPontos = 0;
        }
    }
}

/* Chamada todo frame: verifica se o tempo da CPU acabou (sem sleep!) */
static void atualizarCPU(Jogo *j) {
    if (!j->modoCPU || j->tela != TELA_PERGUNTA || j->cpuRespondeu) return;
    if (mostrarPopupDesistencia || mostrarPopupResposta) return;
    if (GetTime() - j->inicioRodada < j->tempoCPU) return;
    if (!j->modoCPU || j->tela != TELA_PERGUNTA || j->cpuRespondeu) return;
    if (GetTime() - j->inicioRodada < j->tempoCPU) return;

    j->cpuRespondeu = true;
    char certa = letraCorreta(j);

    if (j->respostaCPU == certa) {
        j->pontosCPU++;
        popupAcertou = false;
        snprintf(j->mensagem, sizeof(j->mensagem),
                 "Angela respondeu %c e acertou primeiro!", j->respostaCPU);
        finalizarRodada(j);
    } else if (j->pamRespondeu) {
        popupAcertou = false;
        snprintf(j->mensagem, sizeof(j->mensagem), "Os dois erraram!");
        finalizarRodada(j);
    }
    /* se Pam ainda nao respondeu, ela continua podendo tentar */
}
/* ------------------------------------------------------------------ */
/* AJUDAS                                                              */
/* ------------------------------------------------------------------ */

static void usarAjuda(Jogo *j, int ajuda) {
    if (j->estoque[ajuda] <= 0 || j->usadaNestaPergunta[ajuda] || j->pamRespondeu) return;
    char certa = letraCorreta(j);
    int candidatas[4];
    int total = 0;
    if (ajuda == AJUDA_5050) {
        for (int i = 0; i < 4; i++) {
            if ('A' + i != certa && !j->oculta[i]) candidatas[total++] = i;
        }
        if (total < 2) return;
        /* embaralha e esconde duas */
        for (int i = total - 1; i > 0; i--) {
            int k = rand() % (i + 1);
            int aux = candidatas[i]; candidatas[i] = candidatas[k]; candidatas[k] = aux;
        }
        j->oculta[candidatas[0]] = true;
        j->oculta[candidatas[1]] = true;
    } else if (ajuda == AJUDA_DICA) {
        for (int i = 0; i < 4; i++) {
            if ('A' + i != certa && !j->oculta[i]) candidatas[total++] = i;
        }
        if (total < 1) return;
        j->dica = candidatas[rand() % total];
    } else if (ajuda == AJUDA_TROCA) {
        int idx[1];
        if (sortearPerguntasRodada(j->banco, j->rodada, idx, j->jaUsadas) != 1) {
            snprintf(j->mensagem, sizeof(j->mensagem), "Não há outra pergunta desse nível disponível.");
            return;
        }
        j->indice = idx[0];
        limparEfeitosDaPergunta(j);
        j->usadaNestaPergunta[AJUDA_DICA] = false;
        j->usadaNestaPergunta[AJUDA_5050] = false;
    }
    j->estoque[ajuda]--;
    j->usadaNestaPergunta[ajuda] = true;

}

/* ------------------------------------------------------------------ */
/* TELAS                                                               */
/* ------------------------------------------------------------------ */

static void telaMenu(Jogo *j) {

    /* 1. Fundo de restaurante ocupando toda a tela */

    if (texFundoMenu.id != 0) {
        Rectangle orig = { 0, 0, (float)texFundoMenu.width, (float)texFundoMenu.height };
        Rectangle dest = { 0, 0, (float)LARGURA, (float)ALTURA };

        DrawTexturePro(texFundoMenu, orig, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);

        /* Pelicula escura leve (35%) para melhorar a leitura dos textos sobre a foto */
        DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 0, 0, 0, 90 });
    }

    /* 2. Titulos no topo da tela */

    centralizado("THE OFFICE", 30, 58, COR_TITULO_PRINCIPAL);
    centralizado("The Dundies", 95, 32, COR_SUBTITULO);

    if (j->banco->quantidade == 0) {
        centralizado("Nenhuma pergunta carregada! Confira o perguntas.csv", 140, 20, RED);

    }

    /* 3. Lado Esquerdo: Mensagem + Imagem da Pam */

    const char *msgPam = "Você jogará como Pam";

    int tamMsg = 20;
    int largMsg = MeasureText(msgPam, tamMsg);
    int xCentroEsq = 230; /* Centro da coluna esquerda */
    /* Caixinha escura atras da mensagem para garantir legibilidade */
    Rectangle fundoMsg = { (float)(xCentroEsq - largMsg / 2 - 14), 175, (float)(largMsg + 28), 34 };
    DrawRectangleRounded(fundoMsg, 0.5f, 8, (Color){ 20, 25, 40, 210 });
    DrawText(msgPam, xCentroEsq - largMsg / 2, 182, tamMsg, COR_TEXTO_PAM);
   
    if (texPamMenu.id != 0) {
        float altPam = 480.0f; /* Altura da imagem Pam */
        float proporcao = (float)texPamMenu.width / (float)texPamMenu.height;
        float largPam = altPam * proporcao;
        Rectangle origPam = { 0, 0, (float)texPamMenu.width, (float)texPamMenu.height };
        /* Posiciona centralizada em xCentroEsq (230) e encostada na base da tela (ALTURA - altPam) */
        Rectangle areaPam = { xCentroEsq - largPam / 2.0f, (float)ALTURA - altPam + 35.0f, largPam, altPam };
        DrawTexturePro(texPamMenu, origPam, areaPam, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        Rectangle areaAviso = { 90, 220, 280, 380 };
        DrawRectangleRounded(areaAviso, 0.1f, 8, (Color){ 40, 52, 90, 200 });
        DrawRectangleRoundedLinesEx(areaAviso, 0.1f, 8, 2, COR_TITULO_PRINCIPAL);
        DrawText("Coloque Pam.png em", 125, 380, 18, WHITE);
        DrawText("assets/visuais/", 150, 405, 18, COR_TITULO_PRINCIPAL);
    }

    /* 4. Lado Direito: 3 Botoes arredondados */
    bool temPerguntas = j->banco->quantidade > 0;
    float xBotoes = 540.0f;
    float largBotoes = 340.0f;
    float altBotoes = 62.0f;
    if (botaoArredondado((Rectangle){ xBotoes, 240, largBotoes, altBotoes }, "Jogar Solo", COR_BOTAO_MENU, temPerguntas)) {
        novaPartida(j, false, 0);
    }
    if (botaoArredondado((Rectangle){ xBotoes, 330, largBotoes, altBotoes }, "Competir contra Angela", COR_BOTAO_MENU, temPerguntas)) {
        j->tela = TELA_DIFICULDADE;
    }
    if (botaoArredondado((Rectangle){ xBotoes, 420, largBotoes, altBotoes }, "Sair", COR_BOTAO_SAIR, true)) {
        j->sair = true;
    }
}

static void telaDificuldade(Jogo *j) {
    /* 1. FUNDO EM TOM MARROM (combinando com a madeira do restaurante) */
    ClearBackground((Color){ 54, 34, 24, 255 });

    /* 2. TITULOS NO TOPO */
    centralizado("Dificuldade do Jogo", 35, 44, DOURADO);
    centralizado("(muda apenas a velocidade de resposta da Angela)", 90, 18, (Color){ 235, 225, 205, 255 });

    /* 3. LADO ESQUERDO: ANGELA + BALAO DE FALA */
    float xEsq = 45.0f;
    float largEsq = 435.0f;
    int tamFonteFala = 20;
    const char *falaAngela = "Oi pam, espero que esteja preparada";

    int altTextoFala = calcularAlturaTexto(falaAngela, (int)largEsq - 36, tamFonteFala);
    float altCaixaFala = (float)(altTextoFala + 44);
    if (altCaixaFala < 95.0f) altCaixaFala = 95.0f;

    float yBaseComum = 575.0f;
    float yCaixaFala = yBaseComum - altCaixaFala;
    float altAngela = 350.0f;
    float yTopoAngela = yCaixaFala - altAngela + 15.0f;

    if (texAngela.id != 0) {
        float prop = (float)texAngela.width / (float)texAngela.height;
        float largAngela = altAngela * prop;
        Rectangle origA = { 0, 0, (float)texAngela.width, (float)texAngela.height };
        Rectangle destA = { xEsq + (largEsq - largAngela) / 2.0f, yTopoAngela, largAngela, altAngela };
        DrawTexturePro(texAngela, origA, destA, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }

    /* Balao creme estilo Michael e Creed */
    Rectangle caixaFala = { xEsq, yCaixaFala, largEsq, altCaixaFala };
    DrawRectangleRounded(caixaFala, 0.08f, 8, (Color){ 245, 238, 220, 245 });
    DrawRectangleRoundedLinesEx(caixaFala, 0.08f, 8, 3, (Color){ 60, 45, 35, 255 });

    /* Plaquinha com o nome dela */
    Rectangle placaNome = { xEsq + 18, yCaixaFala - 19.0f, 175, 32 };
    DrawRectangleRounded(placaNome, 0.25f, 8, (Color){ 75, 45, 50, 255 });
    DrawRectangleRoundedLinesEx(placaNome, 0.25f, 8, 2, WHITE);
    DrawText("Angela Martin", (int)placaNome.x + 18, (int)placaNome.y + 7, 18, WHITE);

    textoQuebrado(falaAngela, (int)caixaFala.x + 18, (int)caixaFala.y + 22,
                  (int)caixaFala.width - 36, tamFonteFala, (Color){ 35, 28, 25, 255 });

    /* 4. LADO DIREITO: BOTOES DE DIFICULDADE E VOLTAR */
    float xDir = 535.0f;
    float largDir = 390.0f;
    float altBotao = 62.0f;
    float yInicioBotoes = 210.0f;

    for (int n = 0; n < 3; n++) {
        char texto[64];
        snprintf(texto, sizeof(texto), "%s  (%d a %d s)", NOMES_NIVEL[n], TEMPO_MIN[n], TEMPO_MAX[n]);
        Rectangle rBotao = { xDir, yInicioBotoes + n * 82.0f, largDir, altBotao };

        if (botaoArredondado(rBotao, texto, COR_BOTAO_MENU, true)) {
            novaPartida(j, true, n);
        }
    }

    if (botaoArredondado((Rectangle){ xDir, yInicioBotoes + 3 * 82.0f + 15.0f, largDir, 54.0f }, "Voltar", COR_BOTAO_SAIR, true)) {
        j->tela = TELA_MENU;
    }
}

/* Popup de feedback do modo Solo: e uma tela separada da pergunta. */
/* Popup de feedback (usado tanto no modo Solo quanto contra Angela) */
static void desenharPopupResposta(Jogo *j) {
    const Pergunta *p = &j->banco->itens[j->indice];
    char texto[512];

    /* Cores personalizadas para leitura no fundo creme */
    Color amareloEscuro = { 170, 120, 0, 255 };
    Color pretoOpaco    = { 32, 32, 35, 230 };
    Color textoEscuro   = { 35, 28, 25, 255 };

    /* Fundo do restaurante escurecido */
    if (texFundoMenu.id != 0) {
        Rectangle origem = { 0, 0, (float)texFundoMenu.width, (float)texFundoMenu.height };
        Rectangle destino = { 0, 0, (float)LARGURA, (float)ALTURA };
        DrawTexturePro(texFundoMenu, origem, destino, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        ClearBackground(FUNDO);
    }
    DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 0, 0, 0, 185 });

    /* Quadrado quase ocupando a tela toda */
    Rectangle painel = { 90, 80, 820, 490 };
    DrawRectangleRounded(painel, 0.06f, 10, (Color){245, 238, 220, 225});
    DrawRectangleRoundedLinesEx(painel, 0.06f, 10, 4, popupAcertou ? VERDE : VERMELHO);

    if (popupAcertou) {
        if (j->modoCPU) {
            const char *titulo = "PONTO PARA A PAM!";
            DrawText(titulo, (LARGURA - MeasureText(titulo, 38)) / 2, 145, 38, VERDE);

            DrawText(j->mensagem, (LARGURA - MeasureText(j->mensagem, 24)) / 2, 235, 24, VERDE);

            snprintf(texto, sizeof(texto), "Placar: Pam %d x %d Angela", j->pontosPam, j->pontosCPU);
            DrawText(texto, (LARGURA - MeasureText(texto, 24)) / 2, 295, 24, amareloEscuro);
        } else {
            const char *titulo = "RESPOSTA CORRETA!";
            DrawText(titulo, (LARGURA - MeasureText(titulo, 38)) / 2, 145, 38, VERDE);

            snprintf(texto, sizeof(texto), "Voce ganhou R$ %d!", popupPontos);
            DrawText(texto, (LARGURA - MeasureText(texto, 28)) / 2, 245, 28, VERDE);

            snprintf(texto, sizeof(texto), "Total acumulado: R$ %d", j->premioAcumulado);
            DrawText(texto, (LARGURA - MeasureText(texto, 22)) / 2, 295, 22, amareloEscuro);
        }
    } else {
        if (j->modoCPU) {
            /* Exibe "Angela respondeu X e acertou primeiro!" ou "Os dois erraram!" */
            DrawText(j->mensagem, (LARGURA - MeasureText(j->mensagem, 30)) / 2, 130, 30, VERMELHO);
        } else {
            const char *titulo = "RESPOSTA ERRADA!";
            DrawText(titulo, (LARGURA - MeasureText(titulo, 38)) / 2, 135, 38, VERMELHO);
        }

        const char *sub = "A resposta certa era:";
        DrawText(sub, (LARGURA - MeasureText(sub, 24)) / 2, 195, 24, VERMELHO);

        char certa = letraCorreta(j);
        int indiceCorreto = certa - 'A';
        if (indiceCorreto >= 0 && indiceCorreto < 4) {
            snprintf(texto, sizeof(texto), "%c) %s", certa, p->alternativas[indiceCorreto]);
            Rectangle boxCerta = { 150, 240, 700, 115 };
            DrawRectangleRounded(boxCerta, 0.12f, 8, (Color){ 35, 45, 65, 255 });
            DrawRectangleRoundedLinesEx(boxCerta, 0.12f, 8, 2, VERDE);
            textoQuebrado(texto, (int)boxCerta.x + 20, (int)boxCerta.y + 22,
                          (int)boxCerta.width - 40, 22, VERDE);
        }

        if (j->modoCPU) {
            snprintf(texto, sizeof(texto), "Placar: Pam %d x %d Angela", j->pontosPam, j->pontosCPU);
            DrawText(texto, (LARGURA - MeasureText(texto, 22)) / 2, 385, 22, amareloEscuro);
        }
    }

    if (botaoArredondado((Rectangle){ 380, 455, 240, 55 }, "Prosseguir", popupAcertou ? VERDE : VERMELHO, true)) {
        mostrarPopupResposta = false;
        

        if (j->modoCPU) {
            if (j->rodada >= TOTAL_RODADAS || j->fimDeJogo) {
                j->fimDeJogo = true;
                j->tela = TELA_RESULTADO;
            } else {
                j->rodada++;
                iniciarRodada(j);
            }
        } else {
            if (popupAcertou) {
                if (j->rodada >= TOTAL_RODADAS) {
                    j->premioFinal = j->premioAcumulado;
                    j->fimDeJogo = true;
                    j->tela = TELA_RESULTADO;
                } else {
                    j->rodada++;
                    iniciarRodada(j);
                }
            } else {
                j->fimDeJogo = true;
                j->tela = TELA_RESULTADO;
            }
        }
    }
}
static void desenharPopupDesistencia(Jogo *j) {
    Color textoEscuro   = { 35, 28, 25, 255 };
    Color amareloEscuro = { 170, 120, 0, 255 };

    /* Fundo do restaurante escurecido */
    if (texFundoMenu.id != 0) {
        Rectangle origem = { 0, 0, (float)texFundoMenu.width, (float)texFundoMenu.height };
        Rectangle destino = { 0, 0, (float)LARGURA, (float)ALTURA };
        DrawTexturePro(texFundoMenu, origem, destino, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        ClearBackground(FUNDO);
    }
    DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 0, 0, 0, 185 });

    /* Quadrado central */
    Rectangle painel = { 90, 80, 820, 490 };
    DrawRectangleRounded(painel, 0.06f, 10, (Color){ 245, 238, 220, 225 });
    DrawRectangleRoundedLinesEx(painel, 0.06f, 10, 4, VERMELHO);

    /* Titulo e mensagem de desistencia */
    const char *titulo = "PAM DESISTIU!";
    DrawText(titulo, (LARGURA - MeasureText(titulo, 38)) / 2, 130, 38, VERMELHO);

    const char *sub = "Angela venceu o Dundies por W.O.!";
    DrawText(sub, (LARGURA - MeasureText(sub, 26)) / 2, 195, 26, textoEscuro);

    /* Imagem da Angela (se estiver carregada) */
    if (texAngela.id != 0) {
        float altImg = 175.0f;
        float prop = (float)texAngela.width / (float)texAngela.height;
        float largImg = altImg * prop;
        Rectangle orig = { 0, 0, (float)texAngela.width, (float)texAngela.height };
        Rectangle dest = { (LARGURA - largImg) / 2.0f, 240.0f, largImg, altImg };
        DrawTexturePro(texAngela, orig, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        char placar[64];
        snprintf(placar, sizeof(placar), "Placar no momento: Pam %d x %d Angela", j->pontosPam, j->pontosCPU);
        DrawText(placar, (LARGURA - MeasureText(placar, 22)) / 2, 300, 22, amareloEscuro);
    }

    /* Botao Prosseguir que volta direto para o menu */
    if (botaoArredondado((Rectangle){ 380, 465, 240, 55 }, "Prosseguir", VERMELHO, true)) {
        mostrarPopupDesistencia = false;
        j->fimDeJogo = true;
        j->tela = TELA_MENU;
    }
}

static void telaPergunta(Jogo *j) {

    if (mostrarPopupResposta) {
        desenharPopupResposta(j);
        return;
    }
    if (mostrarPopupDesistencia) {
    desenharPopupDesistencia(j);
    return;
    }

    const Pergunta *p = &j->banco->itens[j->indice];
    char texto[300];

    /* 1. FUNDO DO RESTAURANTE */

    if (texFundoMenu.id != 0) {
        Rectangle orig = { 0, 0, (float)texFundoMenu.width, (float)texFundoMenu.height };
        Rectangle dest = { 0, 0, (float)LARGURA, (float)ALTURA };
        DrawTexturePro(texFundoMenu, orig, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 0, 0, 0, 95 });

    }

    /* 2. BANNER SUPERIOR */
    DrawRectangle(0, 0, LARGURA, 48, (Color){ 128, 70, 27, 255 });
    DrawLine(0, 48, LARGURA, 48, (Color){ 96, 52, 2, 255 });

    snprintf(texto, sizeof(texto), "The Dundies | Pergunta %02d/%d", j->rodada, TOTAL_RODADAS);
    DrawText(texto, 20, 13, 22, DOURADO);

    if (!j->modoCPU) {
        snprintf(texto, sizeof(texto), "Valendo: R$ %d (Parar: R$ %d)", PREMIOS[j->rodada - 1], j->premioAcumulado);
        DrawText(texto, 430, 15, 18, WHITE);
    } else {
        snprintf(texto, sizeof(texto), "Pam %d x %d Angela", j->pontosPam, j->pontosCPU);
        DrawText(texto, 470, 14, 20, WHITE);
    }

    snprintf(texto, sizeof(texto), "Moedas: %02d", j->moedas);
    DrawText(texto, LARGURA - MeasureText(texto, 22) - 20, 13, 22, WHITE);

    /* 3. LADO ESQUERDO: BALAO RESPONSIVO DO MICHAEL + IMAGEM */

    float xEsq = 30.0f;
    float largEsq = 435.0f;
    int tamFontePergunta = 20;
    int altTextoPerg = calcularAlturaTexto(p->enunciado, (int)largEsq - 36, tamFontePergunta);
    float altCaixaPerg = (float)(altTextoPerg + 44);
    if (altCaixaPerg < 95.0f) altCaixaPerg = 95.0f;
    float yBaseComum = 565.0f;
    float yCaixaPerg = yBaseComum - altCaixaPerg;
    float altMic = 350.0f;
    float yTopoMichael = yCaixaPerg - altMic + 15.0f;
    if (texMichael.id != 0) {
        float prop = (float)texMichael.width / (float)texMichael.height;
        float largMic = altMic * prop;
        Rectangle origM = { 0, 0, (float)texMichael.width, (float)texMichael.height };
        Rectangle destM = { xEsq + (largEsq - largMic) / 2.0f, yTopoMichael, largMic, altMic };
        DrawTexturePro(texMichael, origM, destM, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }
    Rectangle caixaPergunta = { xEsq, yCaixaPerg, largEsq, altCaixaPerg };
    DrawRectangleRounded(caixaPergunta, 0.08f, 8, (Color){ 245, 238, 220, 245 });
    DrawRectangleRoundedLinesEx(caixaPergunta, 0.08f, 8, 3, (Color){ 60, 45, 35, 255 });
    Rectangle placaNome = { xEsq + 18, yCaixaPerg - 19.0f, 175, 32 };
    DrawRectangleRounded(placaNome, 0.25f, 8, (Color){ 75, 45, 50, 255 });
    DrawRectangleRoundedLinesEx(placaNome, 0.25f, 8, 2, WHITE);
    DrawText("Michael Scott", (int)placaNome.x + 18, (int)placaNome.y + 7, 18, WHITE);
    textoQuebrado(p->enunciado, (int)caixaPergunta.x + 18, (int)caixaPergunta.y + 22, (int)caixaPergunta.width - 36, tamFontePergunta, (Color){ 35, 28, 25, 255 });

    /* 4. LADO DIREITO: BOTOES ALINHADOS COM O MICHAEL E O BALAO */
    float xDir = 495.0f;
    float largDir = 475.0f;
    int tamFonteAlt = 18;
    float alturasAlt[4];
    float somaAlturas = 0.0f;

    for (int i = 0; i < 4; i++) {
        snprintf(texto, sizeof(texto), "%c) %s", 'A' + i, p->alternativas[i]);
        int altTextoAlt = calcularAlturaTexto(texto, (int)largDir - 28, tamFonteAlt);
        alturasAlt[i] = (float)(altTextoAlt + 22);
        somaAlturas += alturasAlt[i];
    }

    float alturaBlocoDir = somaAlturas + (3 * 12.0f) + 18.0f + 40.0f;
    float yAtual = yBaseComum - alturaBlocoDir;

    for (int i = 0; i < 4; i++) {
        snprintf(texto, sizeof(texto), "%c) %s", 'A' + i, p->alternativas[i]);
        float altBotao = alturasAlt[i];
        Rectangle r = { xDir, yAtual, largDir, altBotao };

        if (j->oculta[i]) {

            DrawRectangleRounded(r, 0.18f, 8, (Color){ 50, 50, 55, 210 });
            DrawRectangleRoundedLinesEx(r, 0.18f, 8, 2, DARKGRAY);
            yAtual += altBotao + 12.0f;
            continue;
        }

        bool ativo = !j->pamRespondeu;
        bool sobre = ativo && CheckCollisionPointRec(GetMousePosition(), r);
        Color corCaixa = !ativo ? GRAY : (sobre ? (Color){ 68, 86, 118, 250 } : (Color){ 42, 54, 78, 240 });
        DrawRectangleRounded(r, 0.18f, 8, corCaixa);
        DrawRectangleRoundedLinesEx(r, 0.18f, 8, 2, WHITE);
        textoQuebrado(texto, (int)r.x + 14, (int)r.y + 11, (int)r.width - 28, tamFonteAlt, WHITE);
        if (sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            responder(j, (char)('A' + i));
            return;
        }
        yAtual += altBotao + 12.0f;
    }

    /* 5. AJUDAS */
    float yAjudas = yAtual + 6.0f;
    for (int a = 0; a < QTD_AJUDAS; a++) {
        bool pode = j->estoque[a] > 0 && !j->usadaNestaPergunta[a] && !j->pamRespondeu;
        snprintf(texto, sizeof(texto), "%s (x%d)", NOMES_CURTOS[a], j->estoque[a]);
        if (botaoArredondado((Rectangle){ xDir + a * 160.0f, yAjudas, 152.0f, 40.0f }, texto, VERDE, pode)) {
            usarAjuda(j, a);
        }
    }
    /* 6. DICA E BOTAO PARAR / SAIR */
    if (j->dica >= 0) {
        snprintf(texto, sizeof(texto), "Dica de Creed: a %c esta errada!", 'A' + j->dica);
        DrawText(texto, 30, 595, 19, DOURADO);
    }

    if (!j->modoCPU) {
        if (botaoArredondado((Rectangle){ LARGURA - 335, ALTURA - 52, 185, 40 }, "Ir ao banheiro", AZUL, !j->pamRespondeu)) {
            j->mensagem[0] = '\0';
            j->tela = TELA_LOJA;
            return;
        }

        if (botaoArredondado((Rectangle){ LARGURA - 135, ALTURA - 52, 115, 40 }, "Parar", VERMELHO, !j->pamRespondeu)) {
            j->premioFinal = j->premioAcumulado;
            j->fimDeJogo = true;
            j->tela = TELA_RESULTADO;
            return;
        }
    } else {
        double decorrido = GetTime() - j->inicioRodada;
        float razao = (float)(decorrido / j->tempoCPU);

        if (razao > 1.0f) razao = 1.0f;
        DrawText("Angela pensando...", 495, 575, 18, WHITE);
        DrawRectangle(495, 600, 320, 18, DARKGRAY);
        DrawRectangle(495, 600, (int)(320 * razao), 18, VERMELHO);
        DrawRectangleLines(495, 600, 320, 18, BLACK);

        if (botaoArredondado((Rectangle){ LARGURA - 135, ALTURA - 52, 115, 40 }, "Sair", VERMELHO, true)) {
            mostrarPopupDesistencia = true;
            j->cpuRespondeu = true;
            j->fimDeJogo = true;
            return;
        }
    }

}

static void telaFeedback(Jogo *j) {
    centralizado("THE OFFICE", 90, 40, DOURADO);
    DrawRectangle(100, 190, 800, 200, PAINEL);
    DrawRectangleLines(100, 190, 800, 200, DOURADO);
    textoQuebrado(j->mensagem, 130, 230, 740, 28, WHITE);
    if (botao((Rectangle){ 350, 450, 300, 60 }, "Continuar", AZUL, true)) {
        if (j->fimDeJogo) {
            j->tela = TELA_RESULTADO;
        } else {
            j->rodada++;
            iniciarRodada(j);
        }
    }
}

static void telaLoja(Jogo *j) {

    char texto[256];
    const char *descricoesPoderes[QTD_AJUDAS] = {
        "Creed sussurra uma alternativa que com certeza esta errada.",
        "Elimina duas alternativas incorretas da pergunta atual.",
        "Troca a pergunta atual por outra nova do mesmo nivel."
    };

    /* 1. FUNDO DO BANHEIRO */
    if (texFundoBanheiro.id != 0) {
        Rectangle orig = { 0, 0, (float)texFundoBanheiro.width, (float)texFundoBanheiro.height };
        Rectangle dest = { 0, 0, (float)LARGURA, (float)ALTURA };
        DrawTexturePro(texFundoBanheiro, orig, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 0, 0, 0, 85 });
    } else {
        ClearBackground(FUNDO);
    }

    /* 2. BANNER SUPERIOR */
    DrawRectangle(0, 0, LARGURA, 48, PAINEL);
    DrawLine(0, 48, LARGURA, 48, DOURADO);
    DrawText("The Dundies | Banheiro Feminino", 20, 13, 22, DOURADO);
    snprintf(texto, sizeof(texto), "Moedas: %02d", j->moedas);
    DrawText(texto, LARGURA - MeasureText(texto, 22) - 20, 13, 22, WHITE);

    /* 3. LADO ESQUERDO: CREED + BALAO DE FALA */

    float xEsq = 30.0f;
    float largEsq = 435.0f;
    int tamFonteFala = 18;
    const char *falaCreed = "Oi Pam, antes que pergunte, sim, eu uso o banheiro feminino "
                            "para fazer o numero dois. Aproveitando que voce esta aqui, "
                            "gostaria de comprar algumas \"ajudinhas\" para as respostas?";

    int altTextoFala = calcularAlturaTexto(falaCreed, (int)largEsq - 32, tamFonteFala);
    float altCaixaFala = (float)(altTextoFala + 38);
    if (altCaixaFala < 110.0f) altCaixaFala = 110.0f;

    float yBaseComum = 565.0f;
    float yCaixaFala = yBaseComum - altCaixaFala;
    float altCreed = 335.0f;
    float yTopoCreed = yCaixaFala - altCreed + 18.0f;

    if (texCreed.id != 0) {
        float prop = (float)texCreed.width / (float)texCreed.height;
        float largCreed = altCreed * prop;
        Rectangle origC = { 0, 0, (float)texCreed.width, (float)texCreed.height };
        Rectangle destC = { xEsq + (largEsq - largCreed) / 2.0f, yTopoCreed, largCreed, altCreed };
        DrawTexturePro(texCreed, origC, destC, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }

    Rectangle caixaFala = { xEsq, yCaixaFala, largEsq, altCaixaFala };
    DrawRectangleRounded(caixaFala, 0.08f, 8, (Color){ 245, 238, 220, 245 });
    DrawRectangleRoundedLinesEx(caixaFala, 0.08f, 8, 3, (Color){ 60, 45, 35, 255 });
    Rectangle placaNome = { xEsq + 18, yCaixaFala - 19.0f, 175, 32 };
    DrawRectangleRounded(placaNome, 0.25f, 8, (Color){ 75, 45, 50, 255 });
    DrawRectangleRoundedLinesEx(placaNome, 0.25f, 8, 2, WHITE);
    DrawText("Creed Bratton", (int)placaNome.x + 18, (int)placaNome.y + 7, 18, WHITE);
    textoQuebrado(falaCreed, (int)caixaFala.x + 16, (int)caixaFala.y + 20,
                  (int)caixaFala.width - 32, tamFonteFala, (Color){ 35, 28, 25, 255 });

    /* 4. LADO DIREITO: CARDS DOS PODERES */

    float xDir = 490.0f;
    float largDir = 480.0f;
    float altCard = 132.0f;
    float espacoCards = 14.0f;
    float alturaTotalCards = (QTD_AJUDAS * altCard) + ((QTD_AJUDAS - 1) * espacoCards);
    float yCardAtual = yBaseComum - alturaTotalCards;

    for (int a = 0; a < QTD_AJUDAS; a++) {
        Rectangle card = { xDir, yCardAtual, largDir, altCard };
        DrawRectangleRounded(card, 0.10f, 8, (Color){ 32, 42, 62, 242 });
        DrawRectangleRoundedLinesEx(card, 0.10f, 8, 2, DOURADO);
        Rectangle boxIcone = { card.x + 12, card.y + 12, 100, 108 };
        if (texPoderes[a].id != 0) {
            float tamanhosIcones[QTD_AJUDAS] = { 170.0f, 68.0f, 88.0f };
            float tamanhoBase = tamanhosIcones[a];
            float propI = (float)texPoderes[a].width / (float)texPoderes[a].height;
            float largI = tamanhoBase * propI;
            float altI = tamanhoBase;

            Rectangle origI = { 0, 0, (float)texPoderes[a].width, (float)texPoderes[a].height };
            Rectangle destI = { boxIcone.x + (boxIcone.width - largI) / 2.0f, boxIcone.y + (boxIcone.height - altI) / 2.0f, largI, altI};
            DrawTexturePro(texPoderes[a], origI, destI, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
        float xTexto = boxIcone.x + boxIcone.width + 12.0f;
        float largTexto = 235.0f;

        DrawText(NOMES_AJUDAS[a], (int)xTexto, (int)card.y + 13, 16, DOURADO);
        textoQuebrado(descricoesPoderes[a], (int)xTexto, (int)card.y + 38, (int)largTexto, 15, WHITE);
        snprintf(texto, sizeof(texto), "Na mochila: %d", j->estoque[a]);
        DrawText(texto, (int)xTexto, (int)card.y + 104, 15, LIGHTGRAY);

        float xColComprar = card.x + card.width - 115.0f;
        snprintf(texto, sizeof(texto), "%d moeda%s", PRECOS[a], PRECOS[a] > 1 ? "s" : "");
        int largPreco = MeasureText(texto, 16);

        DrawText(texto, (int)(xColComprar + (102 - largPreco) / 2.0f), (int)card.y + 24, 16, DOURADO);
        bool temMoedas = (j->moedas >= PRECOS[a]);

        if (botaoArredondado((Rectangle){ xColComprar, card.y + 52, 102, 46 }, "Comprar", VERDE, temMoedas)) {
            j->moedas -= PRECOS[a];
            j->estoque[a]++;
            snprintf(j->mensagem, sizeof(j->mensagem), "Comprou: %s!", NOMES_AJUDAS[a]);
        }
        yCardAtual += altCard + espacoCards;
    }

    if (j->mensagem[0] != '\0') {
        DrawText(j->mensagem, 30, 595, 19, DOURADO);
    }

    /* 5. BOTAO VOLTAR */

    if (botaoArredondado((Rectangle){ LARGURA - 175, ALTURA - 62, 155, 40 }, "Voltar", AZUL, true)) {
        j->mensagem[0] = '\0';
        if (j->pamRespondeu) {
            iniciarRodada(j);
        } else {
            j->tela = TELA_PERGUNTA;
        }
    }
}

static void telaResultado(Jogo *j) {
    char msg[200];

    /* Fundo do restaurante escurecido */
    if (texFundoMenu.id != 0) {
        Rectangle orig = { 0, 0, (float)texFundoMenu.width, (float)texFundoMenu.height };
        Rectangle dest = { 0, 0, (float)LARGURA, (float)ALTURA };
        DrawTexturePro(texFundoMenu, orig, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, LARGURA, ALTURA, (Color){ 0, 0, 0, 185 });
    } else {
        ClearBackground(FUNDO);
    }

    if (j->modoCPU) {
        bool pamVenceu = (j->pontosPam > j->pontosCPU);
        bool angelaVenceu = (j->pontosCPU > j->pontosPam);
        Color corBorda = pamVenceu ? VERDE : (angelaVenceu ? VERMELHO : DOURADO);

        /* Quadrado central onde aparece o aviso e a imagem */
        Rectangle painel = { 140, 65, 720, 520 };
        DrawRectangleRounded(painel, 0.06f, 10, PAINEL);
        DrawRectangleRoundedLinesEx(painel, 0.06f, 10, 4, corBorda);

        Texture2D texVencedor = { 0 };
        const char *nomeFallback = "";

        if (pamVenceu) {
            snprintf(msg, sizeof(msg), "Parabens! Voce ganhou a competicao");
            texVencedor = texPamVitoria;   /* <-- AQUI entra a imagem nova da Pam */
            nomeFallback = "PAM (coloque PamVitoria.png em assets/visuais/)";
        } else if (angelaVenceu) {
            snprintf(msg, sizeof(msg), "Angela ganhou o Dundies");
            texVencedor = texAngela;       /* <-- AQUI entra a imagem da Angela */
            nomeFallback = "ANGELA (coloque Angela.png em assets/visuais/)";
        } else {
            snprintf(msg, sizeof(msg), "Empate na competicao!");
        }

        /* Titulo no topo do quadrado */
        int tamTitulo = 30;
        DrawText(msg, (LARGURA - MeasureText(msg, tamTitulo)) / 2, 95, tamTitulo, corBorda);

        /* Placar final */
        char placar[64];
        snprintf(placar, sizeof(placar), "Placar Final: Pam %d x %d Angela", j->pontosPam, j->pontosCPU);
        DrawText(placar, (LARGURA - MeasureText(placar, 22)) / 2, 140, 22, WHITE);

        /* Imagem da vencedora centralizada dentro do quadrado */
        if (pamVenceu || angelaVenceu) {
            if (texVencedor.id != 0) {
                float altImg = 295.0f;
                float prop = (float)texVencedor.width / (float)texVencedor.height;
                float largImg = altImg * prop;
                Rectangle orig = { 0, 0, (float)texVencedor.width, (float)texVencedor.height };
                Rectangle dest = { (LARGURA - largImg) / 2.0f, 180.0f, largImg, altImg };
                DrawTexturePro(texVencedor, orig, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
            } else {
                Rectangle boxAviso = { 320, 195, 360, 260 };
                DrawRectangleRounded(boxAviso, 0.1f, 8, (Color){ 30, 40, 70, 255 });
                DrawRectangleRoundedLinesEx(boxAviso, 0.1f, 8, 2, DOURADO);
                textoQuebrado(nomeFallback, 340, 300, 320, 18, LIGHTGRAY);
            }
        }

        /* Botao na parte inferior do quadrado */
        if (botaoArredondado((Rectangle){ (LARGURA - 240) / 2.0f, 505, 240, 52 }, "Voltar ao Menu", COR_BOTAO_MENU, true)) {
            j->tela = TELA_MENU;
        }
    } else {
        /* Modo Solo */
        int tamFonte = 28;
        if (j->premioFinal > 0) {
            snprintf(msg, sizeof(msg), "Parabens, voce recebeu o dundie de %d pontos!", j->premioFinal);
            DrawText(msg, (LARGURA - MeasureText(msg, tamFonte)) / 2, 250, tamFonte, DOURADO);
        } else {
            snprintf(msg, sizeof(msg), "Vixi, voce nao conseguiu nenhum dundie...");
            DrawText(msg, (LARGURA - MeasureText(msg, tamFonte)) / 2, 250, tamFonte, LIGHTGRAY);
        }

        if (botaoArredondado((Rectangle){ (LARGURA - 240) / 2.0f, 330, 240, 52 }, "Voltar ao Menu", COR_BOTAO_MENU, true)) {
            j->tela = TELA_MENU;
        }
    }
}

/* ------------------------------------------------------------------ */
/* MAIN                                                                */
/* ------------------------------------------------------------------ */

int main(void) {

    BancoPerguntas banco;
    inicializarBanco(&banco);

    /* mesma logica do main.c de console: tenta o binario, senao o CSV */

    if (!carregarBinario(&banco, "perguntas.dat")) {
        liberarBanco(&banco);
        inicializarBanco(&banco);
        carregarCSV(&banco, "perguntas.csv");
    }


    srand((unsigned int)time(NULL));   /* uma unica vez */
    Jogo jogo;
    memset(&jogo, 0, sizeof(jogo));
    jogo.banco = &banco;
    jogo.tela = TELA_MENU;
    jogo.dica = -1;
    InitWindow(LARGURA, ALTURA, "The Office - Show do Milhao");

    //Menu
    if (FileExists("assets/visuais/restaurante.png")) {
        texFundoMenu = LoadTexture("assets/visuais/restaurante.png");
    }
    if (FileExists("assets/visuais/Pam.png")) {
        texPamMenu = LoadTexture("assets/visuais/Pam.png");
    }
    //Perguntas
    if (FileExists("assets/visuais/michael.png")) {
        texMichael = LoadTexture("assets/visuais/michael.png");
    }
    //Banheiro Creed
    if (FileExists("assets/visuais/banheiro.png")) {
        texFundoBanheiro = LoadTexture("assets/visuais/banheiro.png");
    }
    if (FileExists("assets/visuais/Creed.png")) {
        texCreed = LoadTexture("assets/visuais/Creed.png");
    }
    if (FileExists("assets/visuais/caneca.png")) {
        texPoderes[0] = LoadTexture("assets/visuais/caneca.png");
    }
    if (FileExists("assets/visuais/gelatina.png")) {
        texPoderes[1] = LoadTexture("assets/visuais/gelatina.png");
    }
    if (FileExists("assets/visuais/papel.png")) {
        texPoderes[2] = LoadTexture("assets/visuais/papel.png");
    }

    //Finalização dundies
    if (FileExists("assets/visuais/Pam2.png")) {
        texPamVitoria = LoadTexture("assets/visuais/Pam2.png");
    }
    if (FileExists("assets/visuais/angela.png")) {
        texAngela = LoadTexture("assets/visuais/angela.png");
    }

    SetTargetFPS(60);
    while (!WindowShouldClose() && !jogo.sair) {
        atualizarCPU(&jogo);
        BeginDrawing();
        ClearBackground(FUNDO);
        switch (jogo.tela) {
            case TELA_MENU:        telaMenu(&jogo);        break;
            case TELA_DIFICULDADE: telaDificuldade(&jogo); break;
            case TELA_PERGUNTA:    telaPergunta(&jogo);    break;
            case TELA_FEEDBACK:    telaFeedback(&jogo);    break;
            case TELA_LOJA:        telaLoja(&jogo);        break;
            case TELA_RESULTADO:   telaResultado(&jogo);   break;
        }
        EndDrawing();
    }
    if (texFundoMenu.id != 0) UnloadTexture(texFundoMenu);
    if (texPamMenu.id != 0)   UnloadTexture(texPamMenu);
    if (texMichael.id != 0) UnloadTexture(texMichael);
    if (texFundoBanheiro.id != 0) UnloadTexture(texFundoBanheiro);
    if (texCreed.id != 0)         UnloadTexture(texCreed);
    for (int i = 0; i < 3; i++) {
        if (texPoderes[i].id != 0) UnloadTexture(texPoderes[i]);
    }
    if (texPamMenu.id != 0)    UnloadTexture(texPamMenu);
    if (texPamVitoria.id != 0) UnloadTexture(texPamVitoria);
    if (texAngela.id != 0)     UnloadTexture(texAngela);

    CloseWindow();
    free(jogo.jaUsadas);
    liberarBanco(&banco);
    return 0;
}
