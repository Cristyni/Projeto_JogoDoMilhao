# Projeto_JogoDoMilhao
# THE OFFICE — THE DUNDIES 🏆

Projeto desenvolvido em linguagem C para a disciplina de Fundamentos de Programação 2.
Inspirado no episódio "The Dundies", da série The Office.

## Sobre o jogo

Jogo de perguntas e respostas inspirado no Show do Milhão.

Modos disponíveis:
- Solo: Pam responde às perguntas para conquistar o prêmio de R$ 1 milhão.
- Competição: Pam disputa contra Angela, controlada pelo computador.

O jogador também pode visitar a loja do Creed para comprar ajudas usando moedas conquistadas durante a partida.

## Requisitos

- Windows 10 ou 11 (64 bits)
- MSYS2 UCRT64
- GCC
- GNU Make
- Biblioteca raylib

## Instalação das dependências

Abra o terminal MSYS2 UCRT64 e execute:

```bash
pacman -Syu
```

Se o terminal fechar durante a atualização, abra novamente o MSYS2 UCRT64 e conclua a atualização.

Depois execute:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib make
```


## Como executar o jogo

Abra o terminal MSYS2 UCRT64 na pasta principal do projeto.

### Versão gráfica (principal)

Para compilar e abrir o jogo com a interface gráfica:

```bash
make grafico
```

### Versão de console

Para compilar a versão que funciona diretamente no terminal:

```bash
make
```

Para executar essa versão:

```bash
make run
```

### Recompilar o projeto

Caso tenha feito alterações no código:

```bash
make clean
make grafico
```
