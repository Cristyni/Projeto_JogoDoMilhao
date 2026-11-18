# Makefile para o Jogo do Milhão - The Office
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = jogo_the_office

# Lista de arquivos fonte
SRCS = main.c interface.c arquivo.c pergunta.c jogo.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f *.o $(TARGET) perguntas.dat