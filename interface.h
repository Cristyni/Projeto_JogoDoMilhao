/**
 * @file interface.h
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2026
 * 
 * Include Guard
 * 
 */



#ifndef INTERFACE_H
#define INTERFACE_H

#include "pergunta.h"

void MenuPrincipal(BancoPerguntas *banco);
void MenuGerenciar(BancoPerguntas *banco);
void MenuConsultar(BancoPerguntas *banco);
void cadastrarPergunta(BancoPerguntas *banco);
void pesquisarPergunta(BancoPerguntas *banco);
void consultarPorDificuldade(BancoPerguntas *banco, int dificuldade);
void editarPergunta(BancoPerguntas *banco);
void deletarPergunta(BancoPerguntas *banco);


#endif