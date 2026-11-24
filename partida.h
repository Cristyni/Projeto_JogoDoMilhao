/**
 * @file partida.h
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */


#ifndef PARTIDA_H
#define PARTIDA_H

#include "pergunta.h"
#include "cpu.h"
#include "ajudas.h"

// Modos de jogo
typedef enum {
    MODO_SOLO,
    MODO_COMPETICAO
} ModoPartida;

typedef struct {
    const BancoPerguntas *banco;

    ModoPartida modo;
    NivelCPU nivelCPU;
    Computador angela;
    int angelaPreparada;

    int rodada;
    int indicePergunta;
    int *jaUsadas;

    int acertosPam;
    int respondeuPam;
    int acertosAngela;
    int moedas;
    int premio;

    InventarioAjudas inventario;
    int finalizada;

} PartidaGrafica;


int iniciarPartidaGrafica(
    PartidaGrafica *partida,
    const BancoPerguntas *banco,
    ModoPartida modo,
    NivelCPU nivelCPU
);


void finalizarPartidaGrafica(PartidaGrafica *partida);
int avancarRodada(PartidaGrafica *partida);

int registrarRespostaPam(
PartidaGrafica *partida,
    char resposta,
    int moedasPorAcerto
);

void prepararRespostaAngela( PartidaGrafica *partida,double tempoInicio);
 
int atualizarRespostaAngela(PartidaGrafica *partida,double tempoAtual);

#endif