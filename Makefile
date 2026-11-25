
# Makefile - Jogo do Milhao The Office

CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -I/ucrt64/include

# Versao console
TARGET = dundies.exe
SRCS = main.c interface.c arquivo.c pergunta.c jogo.c

# Versao grafica
TARGET_GRAFICO = jogo_grafico.exe
SRCS_GRAFICO = main_grafico.c arquivo.c pergunta.c cpu.c partida.c ajudas.c# Bibliotecas Raylib
RAYLIB_LIBS = -L/ucrt64/lib -lraylib -lopengl32 -lgdi32 -lwinmm

.PHONY: all run grafico clean

all: $(TARGET)

# Compilar jogo de console
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Compilar jogo grafico
$(TARGET_GRAFICO): $(SRCS_GRAFICO)
	$(CC) $(CFLAGS) $(SRCS_GRAFICO) -o $(TARGET_GRAFICO) $(RAYLIB_LIBS)

# Executar console
run: all
	./$(TARGET)

# Executar jogo grafico
grafico: $(TARGET_GRAFICO)
	./$(TARGET_GRAFICO)

# Limpar executaveis e objetos
clean:
	rm -f *.o $(TARGET) $(TARGET_GRAFICO)
