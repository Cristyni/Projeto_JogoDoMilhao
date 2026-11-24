/**
 * @file cpu.c
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"

// Sorteia o tempo de resposta conforme a dificuldade escolhida
static int sortearTempoResposta(NivelCPU nivel) {
    switch (nivel) {
        case CPU_FACIL:
            return 10 + rand() % 6; 
        case CPU_MEDIO:
            return 6 + rand() % 5;  
        case CPU_DIFICIL:
            return 2 + rand() % 4;  
        default:
            return 6 + rand() % 5;  
    }
}

void prepararCPU(Computador *cpu, NivelCPU nivel, double inicioPergunta) {
    if (cpu == NULL) {
        return;
    }

    // 0-5=A, 6-11=B, 12-17=C, 18-23=D: 25% para cada letra
    int numeroSorteado = rand() % 24;
    cpu->resposta = (char)('A' + numeroSorteado / 6);

    cpu->tempoResposta = (double)sortearTempoResposta(nivel);
    cpu->inicioPergunta = inicioPergunta;
    cpu->respondeu = 0;
}

int atualizarCPU(Computador *cpu, double tempoAtual) {
    if (cpu == NULL || cpu->respondeu) {
        return 0;
    }

    if (tempoAtual - cpu->inicioPergunta >= cpu->tempoResposta) {
        cpu->respondeu = 1;
        return 1;
    }

    return 0;
}
