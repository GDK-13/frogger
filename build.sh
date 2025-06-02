#!/bin/bash

# Cria a pasta bin/Debug se não existir
mkdir -p game/bin/Debug

# Caminho de saída do executável
OUTPUT="game/bin/Debug/frogger"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    gcc -o $OUTPUT \
        game/src/main.c \
        game/src/player.c \
        -Igame/include -Igame/src \
        -Lgame/bin/Debug \
        -lraylib \
        -lm -ldl -lpthread -lrt -lX11 \
        -std=c17 -g -DPLATFORM_DESKTOP
elif [[ "$OSTYPE" == "msys" ]]; then
    OUTPUT="game/bin/Debug/frogger.exe"
    gcc -o $OUTPUT \
        game/src/main.c \
        game/src/player.c \
        -Igame/include -Igame/src \
        -Lgame/bin/Debug \
        -lraylib \
        -std=c17 -g -DPLATFORM_DESKTOP
fi

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running the game..."
    cd game/bin/Debug
    ./frogger
else
    echo "Build failed!"
fi
