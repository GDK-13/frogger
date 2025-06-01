#!/bin/bash

# Caminhos
SRC_DIR="game/src"
BIN_DIR="game/bin/Debug"
RAYLIB_DIR="raylib-master/src"

# Nome do executável
OUTPUT="$BIN_DIR/frogger"

# Cria o diretório de saída se não existir
mkdir -p "$BIN_DIR"

# Compilação
echo "🔧 Compilando o projeto..."
gcc -o "$OUTPUT" \
    $(find $SRC_DIR -name "*.c") \
    -Igame/include -Igame/src \
    -I$RAYLIB_DIR \
    -I$RAYLIB_DIR/external \
    -I$RAYLIB_DIR/external/glfw/include \
    -L. -L$BIN_DIR -L$RAYLIB_DIR \
    -lraylib -lm -ldl -lpthread -lrt -lX11 \
    -std=c17 -g -DPLATFORM_DESKTOP

# Resultado
if [ $? -eq 0 ]; then
    echo "✅ Compilado com sucesso!"
    echo "🟢 Executável: $OUTPUT"
else
    echo "❌ Falha na compilação."
fi
