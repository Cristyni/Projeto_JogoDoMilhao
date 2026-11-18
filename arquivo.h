
#ifndef ARQUIVO_H
#define ARQUIVO_H
/**
 * @file arquivo.h
 * @author victor hugo
 * @brief 
 * @version 0.1
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "pergunta.h"

//carregar perguntas de arquivo CSV
int carregarCSV(BancoPerguntas *banco, const char *nomeArquivo);

//carregar perguntas de arquivo binario
int carregarBinario(BancoPerguntas *banco, const char *nomeArquivo);

//salvar perguntas em arquivo binario
int salvarBinario(const BancoPerguntas *banco, const char *nomeArquivo);

//exportar perguntas para arquivo CSV
int exportarCSV(const BancoPerguntas *banco, const char *nomeArquivo);

#endif
