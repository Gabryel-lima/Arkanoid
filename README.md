# 🎮 Arkanoid (Breakout) - Raylib

Um clone do clássico jogo **Arkanoid** desenvolvido em **C** usando a biblioteca **Raylib**. Este projeto demonstra conceitos fundamentais de programação de jogos como física, colisões, renderização e áudio.

![Arkanoid Gameplay](https://img.shields.io/badge/Status-Completo-brightgreen)
![C Language](https://img.shields.io/badge/Language-C-blue)
![Raylib](https://img.shields.io/badge/Framework-Raylib-orange)
![License](https://img.shields.io/badge/License-MIT-green)

## 📋 Índice

- [🎯 Sobre o Projeto](#-sobre-o-projeto)
- [🚀 Recursos](#-recursos)
- [🎮 Como Jogar](#-como-jogar)
- [🛠️ Instalação e Compilação](#️-instalação-e-compilação)
- [📁 Estrutura do Projeto](#-estrutura-do-projeto)
- [🔧 Tecnologias Utilizadas](#-tecnologias-utilizadas)
- [🎵 Sistema de Áudio](#-sistema-de-áudio)
- [📝 Licença](#-licença)

## 🎯 Sobre o Projeto

Este é um clone fiel do clássico jogo **Arkanoid** (também conhecido como Breakout), desenvolvido como parte de uma série educativa sobre programação de jogos. O projeto demonstra:

- **Física de movimento** da bola com velocidade e direção
- **Sistema de colisões** entre bola, paddle e tijolos
- **Renderização 2D** com cores e formas geométricas
- **Sistema de áudio** com efeitos sonoros
- **Interface de usuário** com pontuação e feedback visual

## 🚀 Recursos

### ✨ Funcionalidades Implementadas

- **🎮 Gameplay Completo**
  - Movimento do paddle com teclado (setas) ou mouse
  - Física realista da bola com rebatidas
  - Sistema de pontuação
  - Múltiplas linhas de tijolos coloridos

- **🎵 Sistema de Áudio**
  - Som de colisão com paddle
  - Som de destruição de tijolos
  - Som de game over
  - Som de reinício do jogo

- **🎨 Interface Visual**
  - Cores vibrantes para os tijolos (gradiente HSV)
  - Bordas visuais do campo de jogo
  - Display de pontuação em tempo real
  - Tela de game over com instruções

- **⚙️ Controles Intuitivos**
  - Setas esquerda/direita para mover o paddle
  - Clique do mouse para posicionamento direto
  - Barra de espaço para reiniciar após game over

## 🎮 Como Jogar

### **Controles:**
- **← →** ou **Mouse**: Mover o paddle
- **Espaço**: Reiniciar jogo (após game over)

### **Objetivo:**
Destrua todos os tijolos coloridos rebatendo a bola com o paddle. Não deixe a bola cair!

### **Pontuação:**
- **10 pontos** por tijolo destruído
- **Game Over** quando a bola toca o fundo da tela

### **Dicas:**
- Use as bordas laterais para rebater a bola
- A posição do impacto no paddle afeta a direção da bola
- Mantenha o paddle sempre em movimento!

## 🛠️ Instalação e Compilação

### **Pré-requisitos:**

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential pkg-config libasound2-dev

# Fedora
sudo dnf install gcc pkg-config alsa-lib-devel

# Arch Linux
sudo pacman -S base-devel pkg-config alsa-lib
```

### **Instalar Raylib:**

```bash
# Clone o repositório do raylib
git clone https://github.com/raysan5/raylib.git
cd raylib

# Compile e instale
mkdir build && cd build
cmake ..
make
sudo make install
```

### **Compilar o Jogo:**

```bash
# Clone este repositório
git clone https://github.com/seu-usuario/arkanoid-raylib.git
cd arkanoid-raylib

# Compile o projeto
make

# Execute o jogo
./arkanoid
```

### **Compilação Manual:**

```bash
gcc -Wall -std=c99 -O2 `pkg-config --cflags raylib` -o arkanoid src/main.c `pkg-config --libs raylib` -lm
```

## 📁 Estrutura do Projeto

```
Arkanoid/
├── src/
│   └── main.c              # Código principal do jogo
├── assets/
│   └── sounds/             # Arquivos de áudio
│       ├── paddle_hit.wav
│       ├── brick_hit.wav
│       ├── game_over.wav
│       └── restart.wav
├── raylib/                 # Biblioteca Raylib (submodule)
├── Makefile               # Script de compilação
├── CMakeLists.txt         # Configuração CMake
└── README.md              # Este arquivo
```

## 🔧 Tecnologias Utilizadas

### **Linguagem e Framework:**
- **C (C99)** - Linguagem de programação principal
- **Raylib** - Biblioteca de jogos multiplataforma

### **Conceitos de Programação:**
- **Estruturas de Dados** - Arrays 2D para tijolos
- **Funções** - Modularização do código
- **Ponteiros** - Manipulação eficiente de dados
- **Loops e Condicionais** - Lógica de jogo
- **Matemática** - Física e colisões

### **Recursos do Raylib:**
- **Renderização 2D** - Retângulos, círculos, texto
- **Sistema de Áudio** - Carregamento e reprodução de sons
- **Input Handling** - Teclado e mouse
- **Timing** - Controle de FPS e delta time
- **Cores** - Sistema HSV para gradientes

## 🎵 Sistema de Áudio

O jogo inclui um sistema de áudio completo com 4 efeitos sonoros:

| Som | Arquivo | Quando Toca |
|-----|---------|-------------|
| **Paddle Hit** | `paddle_hit.wav` | Bola colide com paddle |
| **Brick Hit** | `brick_hit.wav` | Tijolo é destruído |
| **Game Over** | `game_over.wav` | Bola cai (game over) |
| **Restart** | `restart.wav` | Jogo é reiniciado |

### **Implementação:**
```c
// Carregamento de sons
static void LoadSounds(void) {
    paddleHitSound = LoadSound("assets/sounds/paddle_hit.wav");
    brickHitSound = LoadSound("assets/sounds/brick_hit.wav");
    gameOverSound = LoadSound("assets/sounds/game_over.wav");
    restartSound = LoadSound("assets/sounds/restart.wav");
}

// Reprodução
PlaySound(paddleHitSound);
```

## 🎬 Série de Vídeos

Este projeto foi desenvolvido como parte de uma série educativa sobre programação de jogos. Os vídeos cobrem:

1. **Episódio 1:** Introdução e Setup do Ambiente
2. **Episódio 2:** Estruturas Básicas e Renderização
3. **Episódio 3:** Física e Movimento da Bola
4. **Episódio 4:** Sistema de Colisões
5. **Episódio 5:** Interface e Pontuação
6. **Episódio 6:** Sistema de Áudio
7. **Episódio 7:** Polimento e Otimizações

## 🤝 Contribuindo

Contribuições são bem-vindas! Sinta-se à vontade para:

- 🐛 Reportar bugs
- 💡 Sugerir melhorias
- 📝 Melhorar a documentação
- 🎨 Adicionar novos recursos

## 📝 Licença

Este projeto está licenciado sob a **MIT License** - veja o arquivo [LICENSE](LICENSE) para detalhes.

---

## 🎯 Próximos Passos

Algumas ideias para expandir o projeto:

- [ ] **Power-ups** (bola maior, paddle maior, etc.)
- [ ] **Múltiplos níveis** com diferentes layouts
- [ ] **Sistema de vidas**
- [ ] **Efeitos visuais** (partículas, explosões)
- [ ] **Música de fundo**
- [ ] **Sistema de high scores**
- [ ] **Modo multiplayer local**

---

**Desenvolvido com ❤️ usando C e Raylib**

*Um projeto educacional para aprender programação de jogos de forma prática e divertida!* 