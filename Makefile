
# Makefile - Jogo do Milhao The Office

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = dundies.exe

# Arquivos fonte
SRCS = main.c interface.c arquivo.c pergunta.c jogo.c

# Arquivos objeto
OBJS = $(SRCS:.c=.o)

.PHONY: all run clean

all: $(TARGET)

# Junta todos os objetos
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	powershell -NoProfile -Command "Remove-Item -Path '*.o', '$(TARGET)' -ErrorAction SilentlyContinue"
