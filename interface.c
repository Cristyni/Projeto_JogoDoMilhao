#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "arquivo.h"
#include "jogo.h"


/**
 * @file interface.c
 * @author Mafer
 * @brief 
 * @version 0.1
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */


// Limpa os caracteres restantes da entrada
void limparBuffer(void) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

// Le uma linha de texto e remove o Enter
void lerTexto(char texto[], int tamanho) {
    if (fgets(texto, tamanho, stdin) != NULL) {
        size_t posicao = strcspn(texto, "\n");

        if (texto[posicao] == '\n') {
            texto[posicao] = '\0';
        } else {
            limparBuffer();
        }
    }
}
void limparTela(void) {
    system("cls");
}

void MenuPrincipal(BancoPerguntas *banco) {

    //menu principal

    int num;
    do {

        limparTela();

        printf("   THE OFFICE - DUNDIES\n");

        printf("    \nMENU PRINCIPAL: \n");
        printf("1 - Jogar\n");
        printf("2 - Gerenciar perguntas\n");
        printf("3 - Consultar perguntas\n");
        printf("4 - Exportar perguntas\n");
        printf("0 - Salvar e sair\n");
        printf("\ndigite sua opção: ");

        // Verifica se foi digitado um numero inteiro
        if (scanf("%d", &num) != 1) {
            printf("Digite apenas numeros!\n");
            limparBuffer();
            num = -1; // Se não, limpa a entrada e define -1 como opcao invalida
        } else {
            limparBuffer();
        }

        switch (num) {

            case 1:
                iniciarJogo(banco);;
                break;

            case 2:
                MenuGerenciar(banco);
                break;

            case 3:
                MenuConsultar(banco);
                break;

            case 4:
                exportarCSV(banco, "perguntas.csv");
                break;

            case 0:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida!\n");
                break;
        }
        if (num != 0) {
            system("pause");
}

    } while (num != 0);
}


void MenuGerenciar(BancoPerguntas *banco) {

    int num;
    do {

        limparTela();

        printf("    \nGERENCIAR PERGUNTAS\n");

        printf("1 - Cadastrar pergunta\n");
        printf("2 - Listar perguntas\n");
        printf("3 - Pesquisar pergunta\n");
        printf("4 - Alterar pergunta\n");
        printf("5 - Excluir pergunta\n");
        printf("0 - Voltar ao menu principal\n");

        printf("\nDigite sua opcao: ");

        if (scanf("%d", &num) != 1) {
            printf("Digite apenas numeros!\n");
            limparBuffer();
            num = -1;
        } else {
            limparBuffer();
        }

        switch (num) {

            //integrante 1
            case 1:
                cadastrarPergunta(banco);
                break;

            case 2:
                listarPerguntas(banco);
                break;

            case 3:
                pesquisarPergunta(banco);
                break;

            case 4:
                editarPergunta(banco);
                break;

            case 5:
                deletarPergunta(banco);
                break;

            case 0:
                printf("Voltando ao menu principal...\n");
                break;

            default:
                printf("Opcao invalida!\n");
                break;
        }
        if (num != 0) {
            system("pause");
}

    } while (num != 0);
}


void MenuConsultar(BancoPerguntas *banco) {

    int num;

    do {

            limparTela();

        printf("   \nCONSULTAR POR DIFICULDADE\n");

        printf("1 - Muito facil\n");
        printf("2 - Facil\n");
        printf("3 - Medio\n");
        printf("4 - Dificil\n");
        printf("5 - Muito dificil\n");
        printf("0 - Voltar ao menu principal\n");

        printf("\nDigite sua opcao: ");

        if (scanf("%d", &num) != 1) {
            printf("Digite apenas numeros!\n");
            limparBuffer();
            num = -1;
        } else {
            limparBuffer();
        }

        switch (num) {

            case 1:
                printf("Perguntas muito faceis\n");
                consultarPorDificuldade(banco, 1);
                break;

            case 2:
                printf("Perguntas faceis\n");
                consultarPorDificuldade(banco, 2);
                break;

            case 3:
                printf("Perguntas de nivel medio\n");
                consultarPorDificuldade(banco, 3);
                break;

            case 4:
                printf("Perguntas dificeis\n");
                consultarPorDificuldade(banco, 4);
                break;

            case 5:
                printf("Perguntas muito dificeis\n");
                consultarPorDificuldade(banco, 5);
                break;

            case 0:
                printf("Voltando ao menu principal...\n");
                break;

            default:
                printf("Opcao invalida!\n");
                break;
        }
        if (num != 0) {
            system("pause");
}

    } while (num != 0);
}


void cadastrarPergunta(BancoPerguntas *banco) {

    Pergunta nova;
    int dificuldade;
    char letra;

    printf("\n CADASTRAR PERGUNTA \n");

    printf("Digite o enunciado: ");
    lerTexto(nova.enunciado, max_enunciado);

    for (int i = 0; i < QTD_alternativas; i++) {

        printf("Alternativa %c: ", 'A' + i);

        lerTexto(nova.alternativas[i], max_alternativa);
    }

    do {
        printf("Digite a letra correta (A, B, C ou D): ");

        if (scanf(" %c", &letra) != 1) {
            limparBuffer();
            letra = '\0';
        } else {
            limparBuffer();
        }

        if (letra >= 'a' && letra <= 'z') {
            letra = letra - ('a' - 'A');
        }

        if (letra < 'A' || letra > 'D') {
            printf("Letra invalida!\n");
        }

    } while (letra < 'A' || letra > 'D');

    nova.letraCorreta = letra;

    do {
        printf("Digite a dificuldade (1 a 5): ");

        if (scanf("%d", &dificuldade) != 1) {
            printf("Digite apenas numeros\n");
            limparBuffer();
            dificuldade = -1;
        } else {
            limparBuffer();
        }

        if (dificuldade < 1 || dificuldade > 5) {
            printf("Dificuldade invalida!\n");
        }

    } while (dificuldade < 1 || dificuldade > 5);

    nova.dificuldade = dificuldade;

    if (inserirPergunta(banco, nova)) {
        printf("\nPergunta cadastrada com sucesso\n");
    } else {
        printf("\nErro ao cadastrar pergunta\n");
    }
}


void pesquisarPergunta(BancoPerguntas *banco) {

    char termo[max_enunciado];

    printf("\n PESQUISAR PERGUNTA \n");

    if (banco->quantidade == 0) {
        printf("\nNenhuma pergunta cadastrada\n");
        return;
    }

    printf("Digite o enunciado ou parte dele: ");
    lerTexto(termo, max_enunciado);

    if (termo[0] == '\0') {
        printf("\nDigite um termo para pesquisar!\n");
        return;
    }

    int indice = pesquisarPerguntaPorTexto(banco, termo);

    if (indice != -1) {

        printf("\n Pergunta encontrada \n");
        printf("Enunciado: %s\n", banco->itens[indice].enunciado);

        printf("A) %s\n", banco->itens[indice].alternativas[0]);
        printf("B) %s\n", banco->itens[indice].alternativas[1]);
        printf("C) %s\n", banco->itens[indice].alternativas[2]);
        printf("D) %s\n", banco->itens[indice].alternativas[3]);

        printf("Resposta correta: %c\n", banco->itens[indice].letraCorreta);
        printf("Dificuldade: %d\n", banco->itens[indice].dificuldade);

    } else {
        printf("\n pergunta não encontrada \n");
    }
}


void consultarPorDificuldade(BancoPerguntas *banco, int dificuldade) {

    int encontrou = 0;

    for (int i = 0; i < banco->quantidade; i++) {

        if (banco->itens[i].dificuldade == dificuldade) {

            encontrou = 1;

            printf("\nPergunta: %s\n", banco->itens[i].enunciado);

            printf("A) %s\n", banco->itens[i].alternativas[0]);
            printf("B) %s\n", banco->itens[i].alternativas[1]);
            printf("C) %s\n", banco->itens[i].alternativas[2]);
            printf("D) %s\n", banco->itens[i].alternativas[3]);
        }
    }

    if (encontrou == 0) {
        printf("Nenhuma pergunta cadastrada nesse nivel\n");
    }
}

void editarPergunta(BancoPerguntas *banco) {

    char termo[max_enunciado];
    int indice;
    int dificuldade;
    char letra;
    Pergunta atualizada;

    printf("\n EDITAR PERGUNTA \n");

    if (banco->quantidade == 0) {
        printf("\nNenhuma pergunta cadastrada!\n");
        return;
    }

    printf("Digite o enunciado ou parte dele: ");
    lerTexto(termo, max_enunciado);

    if (termo[0] == '\0') {
        printf("\nDigite um termo para pesquisar!\n");
        return;
    }

    indice = pesquisarPerguntaPorTexto(banco, termo);

    if (indice == -1) {
        printf("\nPergunta nao encontrada!\n");
        return;
    }

    printf("\nPergunta encontrada!\n");
    printf("Enunciado: %s\n", banco->itens[indice].enunciado);

    printf("\nDigite o novo enunciado: ");
    lerTexto(atualizada.enunciado, max_enunciado);

    for (int i = 0; i < QTD_alternativas; i++) {

        printf("Nova alternativa %c: ", 'A' + i);

        lerTexto(atualizada.alternativas[i], max_alternativa);
    }

    do {
        printf("Digite a nova letra correta (A, B, C ou D): ");

        scanf(" %c", &letra);
        limparBuffer();

        if (letra >= 'a' && letra <= 'z') {
            letra = letra - ('a' - 'A');
        }

        if (letra < 'A' || letra > 'D') {
            printf("Letra invalida!\n");
        }

    } while (letra < 'A' || letra > 'D');

    atualizada.letraCorreta = letra;

    do {
        printf("Digite a nova dificuldade (1 a 5): ");

        if (scanf("%d", &dificuldade) != 1) {
            printf("Digite apenas numeros!\n");
            limparBuffer();
            dificuldade = -1;
        }
        else {
            limparBuffer();
        }

        if (dificuldade < 1 || dificuldade > 5) {
            printf("Dificuldade invalida!\n");
        }

    } while (dificuldade < 1 || dificuldade > 5);

    atualizada.dificuldade = dificuldade;

    if (alterarPergunta(banco, indice, atualizada)) {
        printf("\nPergunta alterada com sucesso!\n");
    }
    else {
        printf("\nErro ao alterar pergunta!\n");
    }
}

void deletarPergunta(BancoPerguntas *banco) {

    char termo[max_enunciado];
    char confirmacao;
    int indice;

    printf("\n EXCLUIR PERGUNTA \n");

    if (banco->quantidade == 0) {
        printf("\nNenhuma pergunta cadastrada\n");
        return;
    }

    printf("Digite o enunciado ou parte dele: ");
    lerTexto(termo, max_enunciado);

    if (termo[0] == '\0') {
        printf("\nDigite um termo para pesquisar\n");
        return;
    }

    indice = pesquisarPerguntaPorTexto(banco, termo);

    if (indice == -1) {
        printf("\nPergunta nao encontrada\n");
        return;
    }

    printf("\nPergunta encontrada:\n");
    printf("Enunciado: %s\n", banco->itens[indice].enunciado);

    do {
        printf("\nDeseja realmente excluir esta pergunta? (S/N): ");
        scanf(" %c", &confirmacao);
        limparBuffer();

        if (confirmacao >= 'a' && confirmacao <= 'z') {
            confirmacao = confirmacao - ('a' - 'A');
        }

        if (confirmacao != 'S' && confirmacao != 'N') {
            printf("Digite apenas S ou N!\n");
        }

    } while (confirmacao != 'S' && confirmacao != 'N');

    if (confirmacao == 'S') {

        if (excluirPergunta(banco, indice)) {
            printf("\nPergunta excluida com sucesso\n");
        }
        else {
            printf("\nErro ao excluir pergunta\n");
        }

    }
    else {
        printf("\nExclusao cancelada\n");
    }
}
