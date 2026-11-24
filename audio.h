/**
 * @file audio.h
 * @brief Sons curtos do jogo (compra, erro, ajuda).
 *
 * Os arquivos de som sao OPCIONAIS: se algum .wav nao existir, o jogo
 * funciona normalmente, so que em silencio. Coloque os arquivos em:
 *   assets/sons/compra.wav   assets/sons/erro.wav   assets/sons/ajuda.wav
 */

#ifndef AUDIO_H
#define AUDIO_H

typedef enum {
    SOM_COMPRA = 0,   /* compra confirmada        */
    SOM_ERRO,         /* saldo insuficiente / erro */
    SOM_AJUDA,        /* uma ajuda foi usada       */
    QTD_SONS
} SomId;

/** Chamar UMA vez, depois do InitWindow(). */
void audioIniciar(void);

/** Toca um som (se estiver carregado). Seguro chamar sempre. */
void audioTocar(SomId id);

/** Chamar UMA vez, antes do CloseWindow(). */
void audioFinalizar(void);


#endif /* AUDIO_H */
