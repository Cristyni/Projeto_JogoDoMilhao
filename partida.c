/**
 * @file partida.c
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "partida.h"

    int iniciarPartidaGrafica(
        PartidaGrafica *partida,
        const BancoPerguntas *banco,
        ModoPartida modo,
        NivelCPU nivelCPU
    ) {
        if (partida == NULL || banco == NULL || banco->itens == NULL || banco->quantidade == 0) {
            return 0;
        }

        *partida = (PartidaGrafica){0};

        partida->banco = banco;
        partida->modo = modo;
        partida->nivelCPU = nivelCPU;
        partida->rodada = 1;
        partida->indicePergunta = -1;

        // Reserva memoria para marcar perguntas utilizadas
        partida->jaUsadas = calloc( banco->quantidade, sizeof(int)
        );

        if (partida->jaUsadas == NULL) {
            return 0;
        }

        // Inicia o inventario de ajudas de Creed
        inventarioIniciar(&partida->inventario);

        // Sorteia a primeira pergunta
        int indices[1];

        if (sortearPerguntasRodada( banco, partida->rodada,indices, partida->jaUsadas) != 1) {
            free(partida->jaUsadas);
            partida->jaUsadas = NULL;
            return 0;
        }

        partida->indicePergunta = indices[0];

        return 1;
    }
    void finalizarPartidaGrafica(PartidaGrafica *partida) {

        // Verifica se o ponteiro e valido
        if (partida == NULL) {
            return;
        }

        free(partida->jaUsadas);
        partida->jaUsadas = NULL;

        partida->finalizada = 1;
        partida->indicePergunta = -1;
}

int avancarRodada(PartidaGrafica *partida) {

    if (partida == NULL || partida->finalizada || partida->banco == NULL || partida->jaUsadas == NULL) {
        return 0;
    }

    // A partida possui no maximo 15 rodadas
    if (partida->rodada >= 15) {
        partida->finalizada = 1;
        return 0;
    }

    int proximaRodada = partida->rodada + 1;
    int indices[1];

    int quantidade = sortearPerguntasRodada(
        partida->banco,
        proximaRodada,
        indices,
        partida->jaUsadas
    );

    if (quantidade != 1) {
        partida->finalizada = 1;
        return 0;
    }

    partida->rodada = proximaRodada;
    partida->indicePergunta = indices[0];
    partida->respondeuPam = 0;
    partida->angelaPreparada = 0;

    // Permite utilizar as ajudas na nova pergunta
    inventarioNovaPergunta(&partida->inventario);

    return 1;
}

int registrarRespostaPam(
    PartidaGrafica *partida,
    char resposta,
    int moedasPorAcerto
) {
    // Verifica se a partida esta valida
    if (partida == NULL || partida->finalizada || partida->banco == NULL || partida->respondeuPam || partida->indicePergunta < 0
        ||partida->indicePergunta >= partida->banco->quantidade || moedasPorAcerto < 0) { // joguei pra linha de baixo p/ n ficar tão extenso
            return -1;
    }

    resposta = (char)toupper((unsigned char)resposta);

    // Verifica se a alternativa e valida
    if (resposta < 'A' || resposta > 'D') {
        return -1;
    }

    // Acessa a pergunta atual
    const Pergunta *pergunta =
        &partida->banco->itens[partida->indicePergunta];

    partida->respondeuPam = 1;

    if (resposta == toupper((unsigned char)pergunta->letraCorreta)) {

        partida->acertosPam++;
        partida->moedas += moedasPorAcerto;

        return 1;
    }

    return 0;
}

void prepararRespostaAngela(PartidaGrafica *partida, double tempoInicio) {

    if (partida == NULL || partida->finalizada) {
        return;
    }
    // Angela so participa do modo Competicao
    if (partida->modo != MODO_COMPETICAO) {
        return;
    }

    // Evita sortear novamente na mesma pergunta
    if (partida->angelaPreparada) {
        return;
    }

    prepararCPU(
        &partida->angela,
        partida->nivelCPU,
        tempoInicio
    );

    partida->angelaPreparada = 1;
}

int atualizarRespostaAngela(PartidaGrafica *partida,double tempoAtual) {

    if (partida == NULL || partida->finalizada || partida->modo != MODO_COMPETICAO || !partida->angelaPreparada ||
        partida->banco == NULL || partida->indicePergunta < 0 || partida->indicePergunta >= partida->banco->quantidade) {
            return -1;
    }

    if (!atualizarCPU(&partida->angela, tempoAtual)) {
        return 0;
    }

    const Pergunta *pergunta = &partida->banco->itens[partida->indicePergunta];

    // Compara a resposta de Angela com a correta
    if (partida->angela.resposta == toupper((unsigned char)pergunta->letraCorreta)) {
        partida->acertosAngela++;
        return 1; //acertou
    }

    return 2; //errou
}