/**
 * @file audio.c
 * @brief Sons curtos do jogo. Arquivos ausentes = silencio (sem travar).
 */

#include <stdbool.h>
#include "raylib.h"
#include "audio.h"

static const char *ARQUIVOS[QTD_SONS] = {
    "assets/sons/compra.wav",
    "assets/sons/erro.wav",
    "assets/sons/ajuda.wav"
};

static Sound sons[QTD_SONS];       /* variavel estatica: comeca zerada */
static bool audioPronto = false;

void audioIniciar(void) {
    if (audioPronto) {
        return;
    }

    InitAudioDevice();

    if (!IsAudioDeviceReady()) {
        return;                    /* sem placa de som: segue em silencio */
    }

    audioPronto = true;

    for (int i = 0; i < QTD_SONS; i++) {
        if (FileExists(ARQUIVOS[i])) {
            sons[i] = LoadSound(ARQUIVOS[i]);
        }
    }
}

void audioTocar(SomId id) {
    if (!audioPronto || id < 0 || id >= QTD_SONS) {
        return;
    }

    /* frameCount > 0 significa que o som foi carregado de verdade */
    if (sons[id].frameCount > 0) {
        PlaySound(sons[id]);
    }
}

void audioFinalizar(void) {
    if (!audioPronto) {
        return;
    }

    for (int i = 0; i < QTD_SONS; i++) {
        if (sons[i].frameCount > 0) {
            UnloadSound(sons[i]);
            sons[i].frameCount = 0;
        }
    }
    CloseAudioDevice();
    audioPronto = false;
}
