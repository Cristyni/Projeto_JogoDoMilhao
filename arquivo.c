
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

/**
 * @file arquivo.c
 * @author victor hugo
 * @brief 
 * @version 0.1
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */


//CARREGAR PERGUNTAS DO CSV---------
int carregarCSV(BancoPerguntas *banco, const char *nomeArquivo) {

    FILE *arquivo;
    char linha[1024];

    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo CSV!\n");
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        Pergunta nova;

        char *campo;

        // tira quebra de linha
        linha[strcspn(linha, "\r\n")] = '\0';

        // Ler o enunciado
        campo = strtok(linha, ";");

        if (campo == NULL || strlen(campo) >= max_enunciado)
            continue;

        strcpy(nova.enunciado, campo);

        // Ler as quatro alternativas
        int valido = 1;

        for (int i = 0; i < QTD_alternativas; i++) {

            campo = strtok(NULL, ";");

            if (campo == NULL || strlen(campo) >= max_alternativa) {
                valido = 0;
                break;
            }

            strcpy(nova.alternativas[i], campo);
        }

        if (!valido)
            continue;

        // Le a letra correta
        campo = strtok(NULL, ";");

        if (campo == NULL || strlen(campo) != 1)
            continue;

        nova.letraCorreta = campo[0];

        if (nova.letraCorreta < 'A' || nova.letraCorreta > 'D')
            continue;

        // Le a dificuldade
        campo = strtok(NULL, ";");

        if (campo == NULL)
            continue;

        nova.dificuldade = atoi(campo);

        if (nova.dificuldade < 1 || nova.dificuldade > 5)
            continue;

        // coloca a pergunta no banco
        if (!inserirPergunta(banco, nova)) {
            fclose(arquivo);
            return 0;
        }
    }

    fclose(arquivo);

    printf("Perguntas carregadas do CSV!\n");

    return 1;
}

//CARREGAR PERGUNTAS DO BINARIO ---------------------------------------
int carregarBinario(BancoPerguntas *banco, const char *nomeArquivo) {

    FILE *arquivo;

    arquivo = fopen(nomeArquivo, "rb");

    if (arquivo == NULL) {
        return 0;
    }

    int quantidade;

    if (fread(&quantidade, sizeof(int), 1, arquivo) != 1 ||
        quantidade < 0) {
        fclose(arquivo);
        return 0;
    }

    for (int i = 0; i < quantidade; i++) {

        Pergunta nova;

        if (fread(&nova, sizeof(Pergunta), 1, arquivo) != 1) {
            fclose(arquivo);
            return 0;
        }

        if (!inserirPergunta(banco, nova)) {
            fclose(arquivo);
            return 0;
        }
    }

    fclose(arquivo);

    printf("Perguntas carregadas do binario!\n");

    return 1;
}

//SALVAR PERGUNTAS NO BINARIO ---------------
int salvarBinario(const BancoPerguntas *banco, const char *nomeArquivo) {

    FILE *arquivo;

    arquivo = fopen(nomeArquivo, "wb");

    if (arquivo == NULL) {
        printf("Erro ao criar arquivo binario!\n");
        return 0;
    }

    // salvar a quantidade de perguntas
    if (fwrite(&banco->quantidade, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return 0;
    }

    // salva todas as perguntas
    for (int i = 0; i < banco->quantidade; i++) {

        if (fwrite(&banco->itens[i], sizeof(Pergunta), 1, arquivo) != 1) {
            fclose(arquivo);
            return 0;
        }
    }

    fclose(arquivo);

    printf("Perguntas salvas no binario!\n");

    return 1;
}


//EXPORTAR PERGUNTAS PARA CSV ----------------------------------
int exportarCSV(const BancoPerguntas *banco, const char *nomeArquivo) {

    FILE *arquivo;

    arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL) {
        printf("Erro ao criar arquivo CSV!\n");
        return 0;
    }

    for (int i = 0; i < banco->quantidade; i++) {

        fprintf(arquivo, "%s;", banco->itens[i].enunciado);

        for (int j = 0; j < QTD_alternativas; j++) {

            fprintf(arquivo, "%s;", banco->itens[i].alternativas[j]);
        }

        fprintf(arquivo, "%c;", banco->itens[i].letraCorreta);

        fprintf(arquivo, "%d\n", banco->itens[i].dificuldade);
    }

    fclose(arquivo);

    printf("Perguntas exportadas para CSV!\n");

    return 1;
}
