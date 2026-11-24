/**
 * @file ajudas.h
 * @brief Ajudas do Creed: precos, inventario e efeitos (SEM raylib).
 *
 * Este modulo so usa a struct Pergunta e rand(). Nao desenha nada e nao
 * mexe em arquivos. A PARTIDA (integrante 3) e quem guarda o saldo de moedas,
 * troca a pergunta e atualiza pontos/premio. Este modulo apenas:
 *   - diz quanto cada ajuda custa;
 *   - controla o ESTOQUE de ajudas compradas e os USOS por pergunta;
 *   - calcula o EFEITO de cada ajuda (qual alternativa esconder, etc.).
 */

#ifndef AJUDAS_H
#define AJUDAS_H

#include <stdbool.h>
#include "pergunta.h"

/* ------------------------------------------------------------------ */
/* PRECOS: todos ficam aqui, para ajustar num unico lugar.             */
/* ------------------------------------------------------------------ */
#define PRECO_DICA   2   /* caneca de cafe        */
#define PRECO_5050   3   /* gelatina com grampeador */
#define PRECO_TROCA  4   /* folha de papel        */

typedef enum {
    AJUDA_NENHUMA = -1,
    AJUDA_DICA = 0,      /* caneca de cafe: revela uma alternativa errada   */
    AJUDA_5050,          /* gelatina: remove 2 alternativas erradas         */
    AJUDA_TROCA,         /* folha de papel: troca a pergunta (mesmo nivel)  */
    QTD_AJUDAS
} TipoAjuda;

/* Ajudas compradas (estoque) e quais ja foram usadas NA PERGUNTA ATUAL. */
typedef struct {
    int  estoque[QTD_AJUDAS];
    bool usada[QTD_AJUDAS];
} InventarioAjudas;

/* Resultado de usar uma ajuda. Se valido == false, nada foi consumido. */
typedef struct {
    TipoAjuda   tipo;
    bool        valido;
    const char *erro;           /* motivo, quando valido == false (senao NULL) */

    int  ocultas[2];            /* 50/50: indices 0..3 a esconder              */
    int  qtdOcultas;            /* 50/50: quantos foram escondidos (0 ou 2)    */
    int  dica;                  /* dica: indice 0..3 que NAO e a correta, ou -1 */
    bool trocarPergunta;        /* troca: a PARTIDA deve sortear outra pergunta */
} EfeitoAjuda;

/* ---- informacoes de cada ajuda (para a loja e a tela) ---- */
int         ajudaPreco(TipoAjuda tipo);
const char *ajudaNome(TipoAjuda tipo);
const char *ajudaDescricao(TipoAjuda tipo);

/* ---- inventario ---- */

/** Zera estoque e usos. Chamar no inicio de cada partida. */
void inventarioIniciar(InventarioAjudas *inv);

/** Libera o uso das ajudas. Chamar sempre que uma NOVA pergunta aparecer
 *  (inclusive depois de uma troca). O estoque nao e alterado. */
void inventarioNovaPergunta(InventarioAjudas *inv);

/**
 * Compra uma ajuda. Se o saldo for suficiente, soma 1 ao estoque e devolve o
 * CUSTO (> 0). Se nao der, devolve 0 e nada muda.
 * Quem desconta as moedas e a PARTIDA:   saldo -= ajudaComprar(&inv, saldo, t);
 */
int ajudaComprar(InventarioAjudas *inv, int saldo, TipoAjuda tipo);

/**
 * Usa uma ajuda na pergunta atual.
 * @param ocultas  alternativas ja escondidas (vetor de 4) ou NULL.
 * Regras: precisa ter estoque, nao pode repetir na mesma pergunta, e precisa
 * haver alternativas erradas suficientes. Se der certo, consome 1 do estoque.
 */
EfeitoAjuda ajudaUsar(InventarioAjudas *inv, TipoAjuda tipo,
                      const Pergunta *pergunta, const bool ocultas[4]);

/** Desfaz um uso (devolve ao estoque). Util se a troca de pergunta falhar. */
void ajudaDevolver(InventarioAjudas *inv, TipoAjuda tipo);


#endif /* AJUDAS_H */
