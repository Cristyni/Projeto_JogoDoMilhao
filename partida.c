/**
 * @file partida.c
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "partida.h"

    int iniciarPartidaGrafica(
        PartidaGrafica *partida,
        const BancoPerguntas *banco,
        ModoPartida modo,
        NivelCPU nivelCPU
    ) {
        if (partida == NULL || banco == NULL || banco->itens == NULL || banco->quantidade == 0) {
            return 0;
        }

        *partida = (PartidaGrafica){0};

        partida->banco = banco;
        partida->modo = modo;
        partida->nivelCPU = nivelCPU;
        partida->rodada = 1;
        partida->indicePergunta = -1;
        partida->dicaIndice = -1;

        // Reserva memoria para marcar perguntas utilizadas
        partida->jaUsadas = calloc( banco->quantidade, sizeof(int)
        );

        if (partida->jaUsadas == NULL) {
            return 0;
        }

        // Inicia o inventario de ajudas de Creed
        inventarioIniciar(&partida->inventario);

        // Sorteia a primeira pergunta
        int indices[1];

        if (sortearPerguntasRodada( banco, partida->rodada,indices, partida->jaUsadas) != 1) {
            free(partida->jaUsadas);
            partida->jaUsadas = NULL;
            return 0;
        }

        partida->indicePergunta = indices[0];

        return 1;
    }
    void finalizarPartidaGrafica(PartidaGrafica *partida) {

        // Verifica se o ponteiro e valido
        if (partida == NULL) {
            return;
        }

        free(partida->jaUsadas);
        partida->jaUsadas = NULL;

        partida->finalizada = 1;
        partida->indicePergunta = -1;
}

int avancarRodada(PartidaGrafica *partida) {

    if (partida == NULL || partida->finalizada || partida->banco == NULL || partida->jaUsadas == NULL) {
        return 0;
    }

    // A partida possui no maximo 15 rodadas
    if (partida->rodada >= 15) {
        partida->finalizada = 1;
        return 0;
    }

    int proximaRodada = partida->rodada + 1;
    int indices[1];

    int quantidade = sortearPerguntasRodada(
        partida->banco,
        proximaRodada,
        indices,
        partida->jaUsadas
    );

    if (quantidade != 1) {
        partida->finalizada = 1;
        return 0;
    }

    partida->rodada = proximaRodada;
    partida->indicePergunta = indices[0];
    partida->respondeuPam = 0;
    partida->angelaPreparada = 0;
    partida->acertouPamRodada = 0;
    partida->acertouAngelaRodada = 0;
    partida->rodadaPontuada = 0;
    partida->rodadaAvaliada = 0;
    for (int i = 0; i < 4; i++) {
        partida->ocultas[i] = false;
    }
    partida->dicaIndice = -1;

    // Permite utilizar as ajudas na nova pergunta
    inventarioNovaPergunta(&partida->inventario);

    return 1;
}

int registrarRespostaPam(
    PartidaGrafica *partida,
    char resposta,
    int moedasPorAcerto,
    double tempoAtual
) {
    // Verifica se a partida esta valida
    if (partida == NULL || partida->finalizada || partida->banco == NULL || partida->respondeuPam || partida->indicePergunta < 0
        ||partida->indicePergunta >= partida->banco->quantidade || moedasPorAcerto < 0) { // joguei pra linha de baixo p/ n ficar tão extenso
            return -1;
    }

    resposta = (char)toupper((unsigned char)resposta);

    // Verifica se a alternativa e valida
    if (resposta < 'A' || resposta > 'D') {
        return -1;
    }

    // Acessa a pergunta atual
    const Pergunta *pergunta =
        &partida->banco->itens[partida->indicePergunta];

    partida->respondeuPam = 1;
    partida->instanteRespostaPam = tempoAtual;

    if (resposta == toupper((unsigned char)pergunta->letraCorreta)) {

        partida->acertosPam++;
        partida->acertouPamRodada = 1;
        partida->moedas += moedasPorAcerto;
        

        return 1;
    }

    return 0;
}

void prepararRespostaAngela(PartidaGrafica *partida, double tempoInicio) {

    if (partida == NULL || partida->finalizada) {
        return;
    }
    // Angela so participa do modo Competicao
    if (partida->modo != MODO_COMPETICAO) {
        return;
    }

    // Evita sortear novamente na mesma pergunta
    if (partida->angelaPreparada) {
        return;
    }

    prepararCPU(
        &partida->angela,
        partida->nivelCPU,
        tempoInicio
    );

    partida->angelaPreparada = 1;
}

int atualizarRespostaAngela(PartidaGrafica *partida,double tempoAtual) {

    if (partida == NULL || partida->finalizada || partida->modo != MODO_COMPETICAO || !partida->angelaPreparada ||
        partida->banco == NULL || partida->indicePergunta < 0 || partida->indicePergunta >= partida->banco->quantidade) {
            return -1;
    }

    if (!atualizarCPU(&partida->angela, tempoAtual)) {
        return 0;
    }

    const Pergunta *pergunta = &partida->banco->itens[partida->indicePergunta];

    // Compara a resposta de Angela com a correta
    if (partida->angela.resposta == toupper((unsigned char)pergunta->letraCorreta)) {
        partida->acertosAngela++;
        partida->acertouAngelaRodada = 1;
        return 1; //acertou
    }

    return 2; //errou
}

int verificarVencedorRodada(const PartidaGrafica *partida) {

    if (partida == NULL || partida->finalizada || partida->modo != MODO_COMPETICAO) {
        return -1;
    }

    // Aguarda as duas participantes responderem
    if (!partida->respondeuPam || !partida->angelaPreparada || !partida->angela.respondeu) {
        return 0;
    }

    // Apenas Pam acertou
    if (partida->acertouPamRodada && !partida->acertouAngelaRodada) {
        return 1;
    }

    // Apenas Angela acertou
    if (!partida->acertouPamRodada && partida->acertouAngelaRodada) {
        return 2;
    }

    // Nenhuma das duas acertou
    if (!partida->acertouPamRodada && !partida->acertouAngelaRodada) {
        return 3;
    }

    // Se as duas acertaram compara os tempos
    double instanteAngela = partida->angela.inicioPergunta + partida->angela.tempoResposta;

    if (partida->instanteRespostaPam < instanteAngela) {
        return 1;
    }

    if (partida->instanteRespostaPam > instanteAngela) {
        return 2;
    }

    // Se as duas acertaram exatamente no mesmo instante
    return 3;
}

int contabilizarResultadoRodada(PartidaGrafica *partida) {

    if (partida == NULL || partida->finalizada) {
        return -1;
    }

    // Evita contabilizar a mesma rodada duas vezes
    if (partida->rodadaPontuada) {
        return 0;
    }

    int resultado = verificarVencedorRodada(partida);

    // Aguarda as respostas ou identifica erro
    if (resultado <= 0) {
        return resultado;
    }

    if (resultado == 1) {
        partida->pontosPam++;
    }
    else if (resultado == 2) {
        partida->pontosAngela++;
    }

    // Em caso de empate, ninguem recebe pontos
    partida->rodadaPontuada = 1;

    return resultado;
}

int encerrarCompeticao(PartidaGrafica *partida) {

    if (partida == NULL || partida->modo != MODO_COMPETICAO) {
        return -1;
    }
    if (partida->rodada != 15 || !partida->rodadaPontuada) {
        return 0;
    }

    partida->finalizada = 1;

    if (partida->pontosPam > partida->pontosAngela) {
        return 1;
    }
    if (partida->pontosAngela > partida->pontosPam) {
        return 2;
    }

    return 3;
}

int avaliarRodadaSolo(PartidaGrafica *partida) {

    if (partida == NULL || partida->modo != MODO_SOLO || partida->finalizada) {
        return -1;
    }

    if (!partida->respondeuPam || partida->rodadaAvaliada) {
        return 0;
    }

    const int premios[15] = {1000, 2000, 3000, 4000, 5000,10000, 20000, 30000, 40000, 50000,100000, 200000, 300000, 500000, 1000000};

    if (partida->rodada < 1 || partida->rodada > 15) {
        return -1;
    }

    partida->rodadaAvaliada = 1;

    if (partida->acertouPamRodada) {

        partida->premio = premios[partida->rodada - 1];

        if (partida->rodada == 15) {
            partida->finalizada = 1;
            return 3;
        }

        return 1;
    }
    if (partida->rodada == 15) {
        partida->premio = 0;
    } else {
        partida->premio /= 2;
    }
    partida->finalizada = 1;
    
    return 2;
}

int desistirPartidaSolo(PartidaGrafica *partida) {

    // Verifica se a partida existe e esta no Modo Solo
    if (partida == NULL || partida->modo != MODO_SOLO || partida->finalizada) {
        return 0;
    }
    // Impede desistir enquanto uma resposta aguarda avaliacao
    if (partida->respondeuPam && !partida->rodadaAvaliada) {
        return 0;
    }
    // Encerra a partida mantendo o premio acumulado
    partida->finalizada = 1;

    return 1;
}

int comprarAjudaCreed(PartidaGrafica *partida, TipoAjuda tipo) {

    if (partida == NULL || partida->finalizada || partida->jaUsadas == NULL) {
        return 0;
    }
    // Tenta comprar a ajuda usando o saldo de moedas
    int custo = ajudaComprar(&partida->inventario,partida->moedas, tipo);

    // Desconta as moedas somente se a compra deu certo
    if (custo > 0) {
        partida->moedas -= custo;
    }

    return custo;
}

EfeitoAjuda usarAjudaCreed( PartidaGrafica *partida, TipoAjuda tipo) {

    EfeitoAjuda efeito = {0};

    efeito.tipo = tipo;
    efeito.dica = -1;

    // Verifica se Pam ainda pode utilizar ajudas
    if (partida == NULL || partida->finalizada || partida->respondeuPam || partida->banco == NULL || partida->jaUsadas == NULL ||
        partida->indicePergunta < 0 || partida->indicePergunta >= partida->banco->quantidade) {
            efeito.erro = "Ajuda indisponivel.";
            return efeito;
    }

    const Pergunta *pergunta =
        &partida->banco->itens[partida->indicePergunta];

    // Utiliza a funcao criada pela outra integrante
    efeito = ajudaUsar(
        &partida->inventario,
        tipo,
        pergunta,
        partida->ocultas
    );

    // Se nao foi possivel utilizar, retorna o erro
    if (!efeito.valido) {
        return efeito;
    }

    // Gelatina: esconde duas alternativas incorretas
    if (tipo == AJUDA_5050) {
        for (int i = 0; i < efeito.qtdOcultas; i++) {
            int indice = efeito.ocultas[i];

            if (indice >= 0 && indice < 4) {
                partida->ocultas[indice] = true;
            }
        }
    }

    // Caneca: indica uma alternativa incorreta
    else if (tipo == AJUDA_DICA) { partida->dicaIndice = efeito.dica;
    }

    // Folha de papel: sorteia outra pergunta do mesmo nivel
    else if (tipo == AJUDA_TROCA) {
        int indices[1];

        int quantidade = sortearPerguntasRodada( partida->banco, partida->rodada, indices, partida->jaUsadas);

        // Se a troca falhar, devolve a ajuda
        if (quantidade != 1) {
            ajudaDevolver(&partida->inventario, tipo);

            efeito.valido = false;
            efeito.trocarPergunta = false;
            efeito.erro = "Nao ha outra pergunta disponivel.";

            return efeito;
        }

        partida->indicePergunta = indices[0];

        // Reinicia os efeitos visuais das alternativas
        for (int i = 0; i < 4; i++) {
            partida->ocultas[i] = false;
        }

        partida->dicaIndice = -1;

        // Reinicia a resposta de Angela para a nova pergunta
        partida->angelaPreparada = 0;
        partida->angela.respondeu = 0;
        partida->acertouAngelaRodada = 0;

        inventarioNovaPergunta(&partida->inventario);
    }

    return efeito;
}