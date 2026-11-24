/**
 * @file ajudas.c
 * @brief Implementacao das ajudas do Creed (sem raylib).
 *
 * Observacao: usa rand(). O srand() deve ser chamado UMA vez no inicio do
 * programa (a partida/main ja faz isso).
 */

#include <stdlib.h>
#include <ctype.h>
#include "ajudas.h"

/* Textos sem acento por enquanto (a fonte padrao da raylib nao tem acentos). */
static const char *NOMES[QTD_AJUDAS] = {
    "Caneca",
    "Gelatina com grampeador",
    "Folha de papel"
};

static const char *DESCRICOES[QTD_AJUDAS] = {
    "Dica: revela uma alternativa que NAO e a correta.",
    "50/50: remove 2 alternativas erradas.",
    "Troca a pergunta por outra do mesmo nivel."
};

static const int PRECOS[QTD_AJUDAS] = { PRECO_DICA, PRECO_5050, PRECO_TROCA };

static bool tipoValido(TipoAjuda tipo) {
    return tipo >= 0 && tipo < QTD_AJUDAS;
}

/* ------------------------------------------------------------------ */
/* Informacoes                                                         */
/* ------------------------------------------------------------------ */
int ajudaPreco(TipoAjuda tipo) {
    return tipoValido(tipo) ? PRECOS[tipo] : 0;
}

const char *ajudaNome(TipoAjuda tipo) {
    return tipoValido(tipo) ? NOMES[tipo] : "?";
}

const char *ajudaDescricao(TipoAjuda tipo) {
    return tipoValido(tipo) ? DESCRICOES[tipo] : "";
}

/* ------------------------------------------------------------------ */
/* Inventario                                                          */
/* ------------------------------------------------------------------ */
void inventarioIniciar(InventarioAjudas *inv) {
    for (int i = 0; i < QTD_AJUDAS; i++) {
        inv->estoque[i] = 0;
        inv->usada[i] = false;
    }
}

void inventarioNovaPergunta(InventarioAjudas *inv) {
    for (int i = 0; i < QTD_AJUDAS; i++) {
        inv->usada[i] = false;
    }
}

int ajudaComprar(InventarioAjudas *inv, int saldo, TipoAjuda tipo) {
    if (inv == NULL || !tipoValido(tipo)) {
        return 0;
    }

    int preco = PRECOS[tipo];

    if (saldo < preco) {
        return 0;               /* sem saldo: nada muda */
    }

    inv->estoque[tipo]++;
    return preco;               /* a PARTIDA desconta do saldo */
}

void ajudaDevolver(InventarioAjudas *inv, TipoAjuda tipo) {
    if (inv == NULL || !tipoValido(tipo)) {
        return;
    }
    inv->estoque[tipo]++;
    inv->usada[tipo] = false;
}

/* ------------------------------------------------------------------ */
/* Efeitos                                                             */
/* ------------------------------------------------------------------ */
static EfeitoAjuda efeitoVazio(TipoAjuda tipo) {
    EfeitoAjuda e;

    e.tipo = tipo;
    e.valido = false;
    e.erro = NULL;
    e.ocultas[0] = -1;
    e.ocultas[1] = -1;
    e.qtdOcultas = 0;
    e.dica = -1;
    e.trocarPergunta = false;

    return e;
}

/* Guarda em saida[] os indices (0..3) das alternativas ERRADAS que ainda
 * nao foram escondidas. Devolve quantas sao. */
static int listarErradas(const Pergunta *p, const bool ocultas[4], int saida[4]) {
    char certa = (char)toupper((unsigned char)p->letraCorreta);
    int total = 0;

    for (int i = 0; i < 4; i++) {
        bool escondida = (ocultas != NULL) && ocultas[i];

        if ('A' + i != certa && !escondida) {
            saida[total] = i;
            total++;
        }
    }

    return total;
}

EfeitoAjuda ajudaUsar(InventarioAjudas *inv, TipoAjuda tipo,
                      const Pergunta *pergunta, const bool ocultas[4]) {
    EfeitoAjuda e = efeitoVazio(tipo);
    int erradas[4];
    int n;

    if (inv == NULL || !tipoValido(tipo)) {
        e.erro = "Ajuda invalida.";
        return e;
    }
    if (inv->estoque[tipo] <= 0) {
        e.erro = "Voce nao tem essa ajuda. Compre com o Creed!";
        return e;
    }
    if (inv->usada[tipo]) {
        e.erro = "Essa ajuda ja foi usada nesta pergunta.";
        return e;
    }

    switch (tipo) {

        case AJUDA_5050:
            if (pergunta == NULL) {
                e.erro = "Nao ha pergunta atual.";
                return e;
            }
            n = listarErradas(pergunta, ocultas, erradas);
            if (n < 2) {
                e.erro = "Nao ha alternativas erradas suficientes.";
                return e;
            }
            {
                /* sorteia duas erradas DIFERENTES */
                int a = rand() % n;
                int b = rand() % (n - 1);
                if (b >= a) {
                    b++;
                }
                e.ocultas[0] = erradas[a];
                e.ocultas[1] = erradas[b];
                e.qtdOcultas = 2;
            }
            break;

        case AJUDA_DICA:
            if (pergunta == NULL) {
                e.erro = "Nao ha pergunta atual.";
                return e;
            }
            n = listarErradas(pergunta, ocultas, erradas);
            if (n < 1) {
                e.erro = "Nao ha alternativa errada para revelar.";
                return e;
            }
            e.dica = erradas[rand() % n];
            break;

        case AJUDA_TROCA:
            e.trocarPergunta = true;    /* quem sorteia a nova e a PARTIDA */
            break;

        default:
            e.erro = "Ajuda invalida.";
            return e;
    }

    /* deu certo: consome 1 do estoque e marca como usada nesta pergunta */
    inv->estoque[tipo]--;
    inv->usada[tipo] = true;
    e.valido = true;

    return e;
}