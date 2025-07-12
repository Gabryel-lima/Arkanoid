#!/bin/bash
set -e

# Instala dependências necessárias
sudo apt update
sudo apt install -y build-essential cmake git libasound2-dev libpulse-dev libx11-dev libxcursor-dev libxinerama-dev libxrandr-dev libxi-dev libgl1-mesa-dev libegl1-mesa-dev libopenal-dev libvorbis-dev libwebp-dev

# Caminho do diretório raylib (assume que está na raiz do projeto)
RAYLIB_DIR="$(dirname "$0")/raylib"

# Clona raylib se não existir
if [ ! -d "$RAYLIB_DIR" ]; then
    git clone https://github.com/raysan5/raylib.git "$RAYLIB_DIR"
fi

# Compila e instala a raylib
cd "$RAYLIB_DIR"
mkdir -p build
cd build
cmake ..
make -j$(nproc)
sudo make install

# Atualiza cache de bibliotecas
sudo ldconfig

echo "\nRaylib instalada com sucesso!" 