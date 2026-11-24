/**
 * @file main.c
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "pergunta.h"
#include "arquivo.h"
#include "raylib.h"


int main(void) {

    BancoPerguntas banco;

    inicializarBanco(&banco);

    // Tenta carregar as perguntas salvas anteriormente
    int carregouBinario =
        carregarBinario(&banco, "perguntas.dat");

    // Se nao houver banco salvo, carrega o CSV
    if (!carregouBinario || banco.quantidade == 0) {

        liberarBanco(&banco);
        inicializarBanco(&banco);

        printf("\nCarregando perguntas do CSV...\n");

        if (!carregarCSV(&banco, "perguntas.csv")) {
            printf("Erro ao carregar perguntas.csv!\n");
        }
    }

    printf("\nTotal de perguntas: %d\n", banco.quantidade);

    for (int nivel = 1; nivel <= 5; nivel++) {
        int quantidade = 0;

        for (int i = 0; i < banco.quantidade; i++) {
            if (banco.itens[i].dificuldade == nivel) {
                quantidade++;
            }
        }

        printf("Dificuldade %d: %d perguntas\n",
               nivel, quantidade);
    }


    MenuPrincipal(&banco);

    if (banco.quantidade > 0) {
        salvarBinario(&banco, "perguntas.dat");
    }

    liberarBanco(&banco);

    return 0;
}