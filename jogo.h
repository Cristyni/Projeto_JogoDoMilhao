/**
 * @file jogo.h
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef JOGO_H
#define JOGO_H

const char *obterDificuldade(int rodada);
void mostrarRodada(int rodada, int pontuacao);
void mostrarPergunta(const char *enunciado, const char *alternativas[4]);
void iniciarJogo(void);


#endif //JOGO_H