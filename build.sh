#!/bin/bash

# Cria a pasta bin/Debug se não existir
mkdir -p game/bin/Debug
if [ $? -ne 0 ]; then
    echo "Erro ao criar o diretório game/bin/Debug."
    exit 1
fi

# Caminho de saída do executável
OUTPUT="game/bin/Debug/sappo"

# Detecta o sistema operacional e compila
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    COMPILER="gcc"
    $COMPILER -o $OUTPUT \
        game/src/main.c \
        game/src/player.c \
        game/src/animation.c \
        game/src/car.c \
        game/src/hud.c \
        game/src/events.c \
        game/src/trunk.c \
        game/src/turtle.c \
        game/src/screen.c \
        -Igame/include -Igame/src \
        -Lgame/bin/Debug \
        -lraylib \
        -lm -ldl -lpthread -lrt -lX11 \
        -std=c17 -g -DPLATFORM_DESKTOP
elif [[ "$OSTYPE" == "msys" ]]; then
    OUTPUT="game/bin/Debug/sappo.exe"
    COMPILER="gcc"
    $COMPILER -o $OUTPUT \
        game/src/main.c \
        game/src/player.c \
        game/src/animation.c \
        game/src/car.c \
        game/src/hud.c \
        game/src/events.c \
        game/src/trunk.c \
        game/src/turtle.c \
        game/src/screen.c \
        -Igame/include -Igame/src \
        -Lgame/bin/Debug \
        -lraylib \
        -std=c17 -g -DPLATFORM_DESKTOP
else
    echo "Sistema operacional não suportado pelo script."
    exit 2
fi

# Verifica se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running the game..."
    cd game/bin/Debug || { echo "Erro ao acessar a pasta game/bin/Debug."; exit 3; }
    # Verifica se o executável existe antes de rodar
    if [ -f "./sappo" ]; then
        ./sappo
    elif [ -f "./sappo.exe" ]; then
        ./sappo.exe
    else
        echo "Executável não encontrado após compilação."
        exit 4
    fi
else
    echo "Build failed!"
    exit 5
fi