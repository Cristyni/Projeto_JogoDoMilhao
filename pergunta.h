
#ifndef PERGUNTA_H
#define PERGUNTA_H

#define max_enunciado 256
#define max_alternativa 128
#define QTD_alternativas 4

typedef struct {
    char enunciado[max_enunciado];
    char alternativas[QTD_alternativas][max_alternativa];
    char letraCorreta;
    int dificuldade;
} Pergunta;

typedef struct {
    Pergunta *itens;
    int quantidade;
    int capacidade;
} BancoPerguntas;

/**
 * @brief Inicializa o banco de perguntas vazio.
 * @param banco Ponteiro para a estrutura BancoPerguntas.
 */
void inicializarBanco(BancoPerguntas *banco);

/**
 * @brief Insere uma nova pergunta no vetor dinâmico.
 * @param banco Ponteiro para a estrutura BancoPerguntas.
 * @param nova Pergunta a ser adicionada.
 * @return int 1 se inseriu com sucesso, 0 se falhou.
 */
int inserirPergunta(BancoPerguntas *banco, Pergunta nova);

/**
 * @brief Exibe todas as perguntas cadastradas no terminal.
 * @param banco Ponteiro constante para o BancoPerguntas.
 */
void listarPerguntas(const BancoPerguntas *banco);

/**
 * @brief Busca uma pergunta pelo texto no enunciado.
 * @param banco Ponteiro constante para o BancoPerguntas.
 * @param termo Palavra ou frase a ser procurada.
 * @return int Índice da pergunta encontrada ou -1 se não existir.
 */
int pesquisarPerguntaPorTexto(const BancoPerguntas *banco, const char *termo);

/**
 * @brief Substitui os dados de uma pergunta existente.
 * @param banco Ponteiro para a estrutura BancoPerguntas.
 * @param indice Posição da pergunta no vetor.
 * @param atualizada Novos dados da pergunta.
 * @return int 1 se alterou com sucesso, 0 se o índice for inválido.
 */
int alterarPergunta(BancoPerguntas *banco, int indice, Pergunta atualizada);

/**
 * @brief Remove uma pergunta do vetor e reduz a memória se necessário.
 * @param banco Ponteiro para a estrutura BancoPerguntas.
 * @param indice Posição da pergunta a ser excluída.
 * @return int 1 se excluiu com sucesso, 0 se o índice for inválido.
 */
int excluirPergunta(BancoPerguntas *banco, int indice);

/**
 * @brief Libera toda a memória alocada dinamicamente pelo banco.
 * @param banco Ponteiro para a estrutura BancoPerguntas.
 */
void liberarBanco(BancoPerguntas *banco);

#endif