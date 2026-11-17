#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

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

 void MenuPrincipal(void){

    //menu principal

    int num;
    do{

        printf("   THE OFFICE - DUNDIES\n");

        printf("    MENU PRINCIPAL: \n");
        printf("1 - Gerenciar perguntas\n");
        printf("2 - Consultar perguntas\n");
        printf("3 - Exportar perguntas\n");
        printf("0 - Salvar e sair\n");
        printf("\ndigite sua opção: ");


        // Verifica se foi digitado um numero inteiro
            if (scanf("%d", &num) != 1) { 
                printf("Digite apenas numeros!\n");

                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                }

                num = -1; // Se não, limpa a entrada e define -1 como opcao invalida
    }

            switch (num){
            case 1:
                MenuGerenciar();
                break;
            case 2:
                MenuConsultar();
                break;
            case 3://pra exportar o csv
                //MenuExportar();
                break;
            case 0:
                //salvar e sair
                break;
            
            default:
                printf("Opcao invalida!\n");
                break;
            }
            } while (num != 0);
        }

    void MenuGerenciar(void){

        int num;
        do {
                printf("     GERENCIAR PERGUNTAS\n");

                printf("1 - Cadastrar pergunta\n");
                printf("2 - Listar perguntas\n");
                printf("3 - Pesquisar pergunta\n");
                printf("4 - Alterar pergunta\n");
                printf("5 - Excluir pergunta\n");
                printf("0 - Voltar ao menu principal\n");

                printf("\nDigite sua opcao: ");

                    if (scanf("%d", &num) != 1) {
                        printf("Digite apenas numeros!\n");

                        int c;
                        while ((c = getchar()) != '\n' && c != EOF) {
                        }

                        num = -1;
                    }

                switch (num) {
                    //integrante 1
                    case 1:
                        printf("Cadastrar pergunta\n");
                        // Chamar a funcao de cadastro
                        break;

                    case 2:
                        printf("Listar perguntas\n");
                        // Chamar a funcao de listagem
                        break;

                    case 3:
                        printf("Pesquisar pergunta\n");
                        // Chamar a funcao de pesquisa
                        break;

                    case 4:
                        printf("Alterar pergunta\n");
                        // Chamar a funcao de alteracao
                        break;

                    case 5:
                        printf("Excluir pergunta\n");
                        // Chamar a funcao de exclusao
                        break;

                    case 0:
                        printf("Voltando ao menu principal...\n");
                        break;

                    default:
                        printf("Opcao invalida!\n");
                        break;
                }

            } while (num != 0);
        }


    void MenuConsultar(void) {

        int num;


            do {

                printf("    CONSULTAR POR DIFICULDADE\n");

                printf("1 - Muito facil\n");
                printf("2 - Facil\n");
                printf("3 - Medio\n");
                printf("4 - Dificil\n");
                printf("5 - Muito dificil\n");
                printf("0 - Voltar ao menu principal\n");

                printf("\nDigite sua opcao: ");
                
                    if (scanf("%d", &num) != 1) {
                        printf("Digite apenas numeros!\n");

                        int c;
                        while ((c = getchar()) != '\n' && c != EOF) {
                        }

                        num = -1;
                    }

                switch (num) {

                    case 1:
                        printf("Perguntas muito faceis\n");
                        // Exibir perguntas muito faceis
                        break;

                    case 2:
                        printf("Perguntas faceis\n");
                        // Exibir perguntas faceis
                        break;

                    case 3:
                        printf("Perguntas de nivel medio\n");
                        // Exibir perguntas medias
                        break;

                    case 4:
                        printf("Perguntas dificeis\n");
                        // Exibir perguntas dificeis
                        break;

                    case 5:
                        printf("Perguntas muito dificeis\n");
                        // Exibir perguntas muito dificeis
                        break;

                    case 0:
                        printf("Voltando ao menu principal...\n");
                        break;

                    default:
                        printf("Opcao invalida!\n");
                        break;
                }

    } while (num != 0);
}
