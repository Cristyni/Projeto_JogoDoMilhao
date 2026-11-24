/**
 * @file loja.h
 * @brief Tela da loja do Creed (banheiro do restaurante).
 *
 * CONTRATO COM A PARTIDA (integrante 3):
 *   - A loja DESENHA a tela e DETECTA os cliques.
 *   - A loja NAO altera moedas nem estoque. Ela so informa o que o jogador
 *     pediu, e a PARTIDA faz a compra:
 *
 *         AcaoLoja acao = lojaDesenhar(saldo, &inventario);
 *         if (acao.comprou != AJUDA_NENHUMA) {
 *             saldo -= ajudaComprar(&inventario, saldo, acao.comprou);
 *         }
 *         if (acao.continuar) { ... ir para a pergunta ... }
 *
 *   - lojaDesenhar() deve ser chamada DENTRO do BeginDrawing()/EndDrawing()
 *     do loop unico do jogo (nao cria janela nem loop proprio).
 */

#ifndef LOJA_H
#define LOJA_H

#include <stdbool.h>
#include "ajudas.h"

typedef struct {
    TipoAjuda comprou;     /* ajuda que o jogador quer comprar (ou AJUDA_NENHUMA) */
    bool      continuar;   /* true quando clicou em "Seguir para a pergunta"      */
} AcaoLoja;

/** Chamar ao ENTRAR na loja (depois do InitWindow). Carrega imagens opcionais
 *  e limpa a mensagem de compra. Pode ser chamada varias vezes. */
void lojaIniciar(void);

/** Desenha a loja e le os cliques deste frame. */
AcaoLoja lojaDesenhar(int saldo, const InventarioAjudas *inv);

/** Libera as imagens. Chamar antes do CloseWindow(). */
void lojaFinalizar(void);


#endif /* LOJA_H */
