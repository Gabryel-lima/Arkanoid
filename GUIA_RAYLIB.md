# Guia de Instalação e Uso da Raylib

## Instalação Global (Recomendada)

1. **Execute o script de instalação:**
   ```sh
   bash install_raylib.sh
   ```
   Isso irá instalar todas as dependências e a raylib no seu sistema Linux.

2. **Compilando seu jogo:**
   - Se o seu Makefile já está configurado, basta rodar:
     ```sh
     make
     ```
   - Ou compile manualmente:
     ```sh
     gcc -o arkanoid src/main.c -lraylib -lm -lpthread -ldl -lrt -lX11
     ```

3. **Executando o jogo:**
   ```sh
   ./arkanoid
   ```

---

## Instalação Local (Somente no Projeto)

Se não quiser instalar a raylib globalmente, você pode compilar a raylib como parte do seu projeto:

1. **Compile a raylib na pasta local:**
   ```sh
   cd raylib
   mkdir -p build
   cd build
   cmake ..
   make -j$(nproc)
   ```
   Isso irá gerar a biblioteca em `raylib/build/raylib/libraylib.a`.

2. **Compile seu jogo linkando com a raylib local:**
   ```sh
   gcc -o arkanoid src/main.c raylib/build/raylib/libraylib.a -Iraylib/src -lm -lpthread -ldl -lrt -lX11
   ```

---

## Observações
- Se instalar globalmente, pode usar `-lraylib` normalmente.
- Se usar local, aponte para o `.a` e inclua o diretório de headers com `-Iraylib/src`.
- Se tiver dúvidas, consulte o [site oficial da raylib](https://www.raylib.com/). 