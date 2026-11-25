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
#include "partida.h"


/*Constantes de preços e regras*/
#define LARGURA 1000
#define ALTURA 650
#define TOTAL_RODADAS 15
#define MOEDAS_POR_ACERTO 1

static const int PRECOS[QTD_AJUDAS] = { 2, 3, 4 };
static const char *NOMES_AJUDAS[QTD_AJUDAS] = {
    "Caneca de cafe (dica)",
    "Gelatina com grampeador (50/50)",
    "Folha de papel (trocar pergunta)"
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

/* Cores e testuras do MENU */
static const Color COR_TITULO_PRINCIPAL = { 240, 190, 40, 255 };  /* Dourado ("THE OFFICE") */
static const Color COR_SUBTITULO        = { 255, 255, 255, 255 }; /* Branco ("The Dundies") */
static const Color COR_TEXTO_PAM        = { 255, 255, 255, 255 }; /* Texto acima da Pam */
static const Color COR_BOTAO_MENU       = { 138, 154, 91, 255 };  /* Azul dos botoes Jogar */
static const Color COR_BOTAO_SAIR       = { 183, 65, 14, 255 };   /* Vermelho do botao Sair */
static const Color COR_TEXTO_BOTAO      = { 255, 255, 255, 255 }; /* Cor das letras dentro do botão */

static Texture2D texFundoMenu = { 0 };
static Texture2D texPamMenu   = { 0 };
static Texture2D texMichael = { 0 };


/*Estado do jogo*/

typedef enum {
    TELA_MENU,
    TELA_DIFICULDADE,
    TELA_PERGUNTA,
    TELA_FEEDBACK,
    TELA_LOJA,
    TELA_RESULTADO
} Tela;

typedef struct {
    BancoPerguntas *banco;
    Tela tela;
    bool sair;
    PartidaGrafica partida;

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
    Computador angela;
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
 /* Desenha um botao arredondado e devolve true se foi clicado */
    static bool botaoArredondado(Rectangle r, const char *texto, Color cor, bool ativo) {
        bool sobre = ativo && CheckCollisionPointRec(GetMousePosition(), r);
        Color c = !ativo ? GRAY : (sobre ? ColorBrightness(cor, 0.25f) : cor);

        /* 0.4f controla o nivel de arredondamento (0.0 = quadrado, 1.0 = pilula) */
        DrawRectangleRounded(r, 0.4f, 12, c);
        DrawRectangleRoundedLinesEx(r, 0.4f, 12, 2, BLACK);

        int larguraTexto = MeasureText(texto, 20);
        DrawText(texto,
                (int)(r.x + (r.width - larguraTexto) / 2),
                (int)(r.y + (r.height - 20) / 2),
                20, COR_TEXTO_BOTAO);

        return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* Desenha um botao e devolve true se foi clicado neste frame */
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

/* Botao de alternativa: texto longo quebrado dentro da caixa */
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
        snprintf(j->mensagem, sizeof(j->mensagem),
                 "Perguntas insuficientes para continuar.");
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
        prepararCPU(
            &j->angela,
            (NivelCPU)j->nivelCPU,
            GetTime()
        );

        j->respostaCPU = j->angela.resposta;
        j->tempoCPU = (int)j->angela.tempoResposta;
        j->inicioRodada = j->angela.inicioPergunta;
    } else {
        j->inicioRodada = GetTime();
    }

    j->inicioRodada = GetTime();   /* o cronometro comeca aqui, depois da loja */
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
    for (int i = 0; i < QTD_AJUDAS; i++) j->estoque[i] = 0;
    j->mensagem[0] = '\0';

    if (j->jaUsadas == NULL) {
        j->tela = TELA_MENU;
        return;
    }
    iniciarRodada(j);
}

/* Encerra a rodada atual e vai para a tela de feedback */
static void finalizarRodada(Jogo *j) {
    if (j->rodada >= TOTAL_RODADAS) j->fimDeJogo = true;
    j->tela = TELA_FEEDBACK;
}

/* Pam clicou em uma alternativa */
static void responder(Jogo *j, char letra) {
    char certa = letraCorreta(j);
    j->pamRespondeu = true;

    if (letra == certa) {
        j->moedas += MOEDAS_POR_ACERTO;

        if (j->modoCPU) {
            j->pontosPam++;
            snprintf(j->mensagem, sizeof(j->mensagem),
                     "Pam acertou primeiro! +1 ponto e +%d moeda.", MOEDAS_POR_ACERTO);
        } else {
            j->acertos++;
            j->premioAcumulado = PREMIOS[j->rodada - 1];
            if (j->rodada == TOTAL_RODADAS) {
                j->premioFinal = j->premioAcumulado;
                snprintf(j->mensagem, sizeof(j->mensagem),
                         "PARABENS! Voce ganhou R$ %d!", j->premioFinal);
            } else {
                snprintf(j->mensagem, sizeof(j->mensagem),
                         "RESPOSTA CORRETA! Premio: R$ %d", j->premioAcumulado);
            }
        }
        finalizarRodada(j);
    } else {
        if (j->modoCPU) {
            if (j->cpuRespondeu) {
                /* a CPU ja tinha errado antes */
                snprintf(j->mensagem, sizeof(j->mensagem),
                         "Os dois erraram. Resposta correta: %c", certa);
                finalizarRodada(j);
            }
            /* senao: espera a CPU tentar (atualizarCPU cuida disso) */
        } else {
            j->premioFinal = (j->rodada == TOTAL_RODADAS) ? 0 : j->premioAcumulado / 2;
            j->fimDeJogo = true;
            snprintf(j->mensagem, sizeof(j->mensagem),
                     "RESPOSTA ERRADA! Correta: %c. Voce leva R$ %d", certa, j->premioFinal);
            finalizarRodada(j);
        }
    }
}

/* Chamada todo frame: verifica se o tempo da CPU acabou (sem sleep!) */
static void atualizarCPUVisual(Jogo *j){
    if (!j->modoCPU || j->tela != TELA_PERGUNTA || j->cpuRespondeu) return;
        if (!atualizarCPU(&j->angela, GetTime())) return;
    j->cpuRespondeu = true;
    char certa = letraCorreta(j);

    if (j->respostaCPU == certa) {
        j->pontosCPU++;
        snprintf(j->mensagem, sizeof(j->mensagem),
                 "Angela respondeu %c e acertou primeiro! Ponto para a Angela.", j->respostaCPU);
        finalizarRodada(j);
    } else if (j->pamRespondeu) {
        snprintf(j->mensagem, sizeof(j->mensagem),
                 "Os dois erraram. Resposta correta: %c", certa);
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
            snprintf(j->mensagem, sizeof(j->mensagem),
                     "Nao ha outra pergunta desse nivel disponivel.");
            return;   /* nao gasta a ajuda */
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

    /* Area da imagem da Pam (abaixo da mensagem) */
   /* Area da imagem da Pam (mantendo a proporcao original sem esticar) */
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
        DrawText("Coloque pam.png em", 125, 380, 18, WHITE);
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
    centralizado("Dificuldade da CPU", 100, 40, WHITE);
    centralizado("(muda apenas a velocidade de resposta do computador)", 155, 18, LIGHTGRAY);

    for (int n = 0; n < 3; n++) {
        char texto[64];
        snprintf(texto, sizeof(texto), "%s  (%d a %d s)", NOMES_NIVEL[n], TEMPO_MIN[n], TEMPO_MAX[n]);
        if (botao((Rectangle){ 330, (float)(230 + n * 85), 340, 65 }, texto, AZUL, true)) {
            novaPartida(j, true, n);
        }
    }
    if (botao((Rectangle){ 330, 500, 340, 50 }, "Voltar", VERMELHO, true)) {
        j->tela = TELA_MENU;
    }
}

static void telaPergunta(Jogo *j) {
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
    DrawLine(0, 48, LARGURA, 48, (Color){ 92, 26, 20, 255 });

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

    /* Base comum alinhada para os dois lados */
    float yBaseComum = 565.0f;
    float yCaixaPerg = yBaseComum - altCaixaPerg;

    float altMic = 360.0f;
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

    textoQuebrado(p->enunciado, (int)caixaPergunta.x + 18, (int)caixaPergunta.y + 22,
                  (int)caixaPergunta.width - 36, tamFontePergunta, (Color){ 35, 28, 25, 255 });

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

    /* 5. AJUDAS (alinhadas na mesma linha de base do balao do Michael) */
    float yAjudas = yAtual + 6.0f;
    for (int a = 0; a < QTD_AJUDAS; a++) {
        bool pode = j->estoque[a] > 0 && !j->usadaNestaPergunta[a] && !j->pamRespondeu;
        snprintf(texto, sizeof(texto), "%s (x%d)", NOMES_CURTOS[a], j->estoque[a]);
        if (botaoArredondado((Rectangle){ xDir + a * 160.0f, yAjudas, 152.0f, 40.0f }, texto, VERDE, pode)) {
            usarAjuda(j, a);
        }
    }

    /* 6. DICA E BOTAO PARAR */
    if (j->dica >= 0) {
        snprintf(texto, sizeof(texto), "Dica de Creed: a %c esta errada!", 'A' + j->dica);
        DrawText(texto, 30, 595, 19, DOURADO);
    }

    if (!j->modoCPU) {
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
            j->tela = TELA_LOJA;
        }
    }
}

static void telaLoja(Jogo *j) {
    char texto[128];

    DrawRectangle(0, 0, LARGURA, 60, PAINEL);
    DrawText("BANHEIRO FEMININO  -  LOJA DO CREED", 20, 16, 24, DOURADO);
    snprintf(texto, sizeof(texto), "MOEDAS: %02d", j->moedas);
    DrawText(texto, LARGURA - 170, 16, 24, WHITE);

    centralizado("\"Psiu... quer uns poderes? Tenho preco bom.\"  - Creed", 90, 22, LIGHTGRAY);

    for (int a = 0; a < QTD_AJUDAS; a++) {
        float y = (float)(150 + a * 90);
        DrawRectangle(100, (int)y, 800, 75, PAINEL);

        snprintf(texto, sizeof(texto), "%s", NOMES_AJUDAS[a]);
        DrawText(texto, 120, (int)y + 12, 22, WHITE);
        snprintf(texto, sizeof(texto), "Preco: %d moedas   |   Voce tem: %d", PRECOS[a], j->estoque[a]);
        DrawText(texto, 120, (int)y + 42, 18, LIGHTGRAY);

        if (botao((Rectangle){ 720, y + 12, 160, 50 }, "Comprar", VERDE, true)) {
            if (j->moedas >= PRECOS[a]) {
                j->moedas -= PRECOS[a];
                j->estoque[a]++;
                snprintf(j->mensagem, sizeof(j->mensagem), "Compra feita: %s!", NOMES_AJUDAS[a]);
            } else {
                snprintf(j->mensagem, sizeof(j->mensagem), "Moedas insuficientes!");
            }
        }
    }

    if (j->mensagem[0] != '\0') {
        DrawText(j->mensagem, 100, 430, 22, DOURADO);
    }

    snprintf(texto, sizeof(texto), "Seguir para a pergunta %d", j->rodada);
    if (botao((Rectangle){ 330, 500, 340, 60 }, texto, AZUL, true)) {
        j->mensagem[0] = '\0';
        iniciarRodada(j);
    }
}

static void telaResultado(Jogo *j) {
    char msg[200];
    int tamFonte = 28;

    /* Fundo simples por enquanto (depois decoramos) */
    ClearBackground(FUNDO);

    if (j->premioFinal > 0) {
        snprintf(msg, sizeof(msg), "Parabéns, você recebeu o dundie de %d pontos!", j->premioFinal);
        int largTexto = MeasureText(msg, tamFonte);
        DrawText(msg, (LARGURA - largTexto) / 2, 250, tamFonte, DOURADO);
    } else {
        snprintf(msg, sizeof(msg), "Vixi, você não conseguiu nenhum dundie...");
        int largTexto = MeasureText(msg, tamFonte);
        DrawText(msg, (LARGURA - largTexto) / 2, 250, tamFonte, LIGHTGRAY);
    }

    /* Botao centralizado logo abaixo da mensagem para voltar ao menu */
    if (botaoArredondado((Rectangle){ (LARGURA - 240) / 2.0f, 330, 240, 52 }, "Voltar ao Menu", COR_BOTAO_MENU, true)) {
        j->tela = TELA_MENU;
    }
}

/* ------------------------------------------------------------------ */
/* MAIN                                                                */
/* ------------------------------------------------------------------ */
int main(void) {
    BancoPerguntas banco;
    inicializarBanco(&banco);

    /* mesma logica do main.c de console: tenta o binario, senao o CSV */
    if (!carregarBinario(&banco, "perguntas.dat") || banco.quantidade == 0) {
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
    if (FileExists("assets/visuais/restaurante.png")) {
        texFundoMenu = LoadTexture("assets/visuais/restaurante.png");
    }
    if (FileExists("assets/visuais/pam.png")) {
        texPamMenu = LoadTexture("assets/visuais/Pam.png");
    }
    if (FileExists("assets/visuais/michael.png")) {
        texMichael = LoadTexture("assets/visuais/michael.png");
    }
    SetTargetFPS(60);

    while (!WindowShouldClose() && !jogo.sair) {

        atualizarCPUVisual(&jogo);

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

    CloseWindow();

    free(jogo.jaUsadas);
    liberarBanco(&banco);
    return 0;
}
