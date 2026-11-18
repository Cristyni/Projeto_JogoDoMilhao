#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "pergunta.h"
#include "arquivo.h"


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

 //main temporaria pra teste das funções do interface.c


int main(void) {

    BancoPerguntas banco;

    inicializarBanco(&banco);
    carregarBinario(&banco, "perguntas.dat");
    MenuPrincipal(&banco);
    salvarBinario(&banco, "perguntas.dat");
    liberarBanco(&banco);

    return 0;
}