/**
 * @file cpu.h
 * @author Mafer
 * @brief Competição com pc
 * @version 0.1
 * @date 2026-11-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef CPU_H
#define CPU_H

typedef enum {
    CPU_FACIL,
    CPU_MEDIO,
    CPU_DIFICIL
} NivelCPU;

typedef struct {
    char resposta;         // Alternativa sorteada: A, B, C ou D
    double tempoResposta;  // Segundos ate o PC responder
    double inicioPergunta; // Instante de inicio da pergunta
    int respondeu;         // 0 = aguardando; 1 = respondeu 
} Computador;

/**
 * @brief Sorteia uma alternativa e o tempo de resposta do PC
 * @param cpu Ponteiro para o estado do computador
 * @param nivel Dificuldade escolhida: facil, medio ou dificil
 * @param inicioPergunta Instante em segundos (ex: GetTime() da raylib)
 * @note Chame srand((unsigned int)time(NULL)) uma vez no inicio do programa
 */
void prepararCPU(Computador *cpu, NivelCPU nivel, double inicioPergunta);

/**
 * @brief Verifica se chegou a hora de o computador responder
 * @param cpu Ponteiro para o estado do computador
 * @param tempoAtual Instante atual em segundos (ex: GetTime() da raylib)
 * @return 1 somente quando o PC acaba de responder; 0 caso contrario.
 */
int atualizarCPU(Computador *cpu, double tempoAtual);

#endif /* CPU_H */
