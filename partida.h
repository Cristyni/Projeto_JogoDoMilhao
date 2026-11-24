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

#include <stdbool.h>
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
    double instanteRespostaPam; //armazena o momento em que Pam escolheu sua alternativa
    int acertosAngela;
    int moedas;
    int premio;

    int acertouPamRodada;
    int acertouAngelaRodada;
    int pontosPam;
    int pontosAngela;
    int rodadaPontuada;
    int rodadaAvaliada;

    bool ocultas[4]; //alternativas que foram escondidas
    int dicaIndice;

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
    int moedasPorAcerto,
    double tempoAtual
);

void prepararRespostaAngela( PartidaGrafica *partida,double tempoInicio);
 
int atualizarRespostaAngela(PartidaGrafica *partida,double tempoAtual);
int verificarVencedorRodada(const PartidaGrafica *partida);
int contabilizarResultadoRodada(PartidaGrafica *partida);
int encerrarCompeticao(PartidaGrafica *partida);
int avaliarRodadaSolo(PartidaGrafica *partida);
int desistirPartidaSolo(PartidaGrafica *partida);
int comprarAjudaCreed(PartidaGrafica *partida, TipoAjuda tipo);

EfeitoAjuda usarAjudaCreed( PartidaGrafica *partida, TipoAjuda tipo, double tempoAtual);
const Pergunta *obterPerguntaAtual( const PartidaGrafica *partida );

int atualizarPartidaCompeticao( PartidaGrafica *partida, double tempoAtual);

#endif