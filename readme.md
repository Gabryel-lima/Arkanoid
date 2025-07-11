A seguir você encontra um roteiro completo (mas enxuto) para criar um **Arkanoid** em C usando **raylib**, com atenção especial para que o mesmo código-fonte possa ser compilado em Windows, Linux e macOS (e até Android, com um passo extra).

---

## 1. Estrutura mínima do projeto

```
arkanoid/
├─ CMakeLists.txt
├─ assets/
│  ├─ paddle.png
│  ├─ ball.png
│  └─ brick.png          # (opcional – podemos desenhar retângulos no código)
└─ src/
   └─ main.c
```

Usaremos **CMake** porque ele abstrai as diferenças entre toolchains e IDEs; basta ter raylib instalado (ou compilá-lo como subdiretório).

---

## 2. Instalando o raylib

### Linux (Ubuntu ≥ 22.04)

```bash
sudo apt install libraylib-dev         # pacote oficial
# ou, se quiser versão mais recente:
git clone https://github.com/raysan5/raylib.git
cd raylib && cmake -B build && cmake --build build
sudo cmake --install build             # instala em /usr/local
```

### Windows

1. Instale **MSYS2** (mingw-64) e abra o terminal *MSYS2 MinGW 64-bit*
2. `pacman -Syu mingw-w64-x86_64-raylib mingw-w64-x86_64-gcc cmake ninja`
3. No *PowerShell* ou VS Code use o kit *MinGW* que o CMake Tools detectará.

### macOS (Homebrew)

```bash
brew install raylib                   # já traz .pc / CMake config
```

*(Para Android, habilite `BUILD_EXAMPLES=ON` no repositório do raylib e gere com `cmake -DBUILD_ANDROID=ON` – depois converta o output em um projeto Gradle; não entraremos nos detalhes agora.)*

---

## 3. Arquivo `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.15)
project(arkanoid C)

# Localize a instalação do raylib (ou use add_subdirectory se o código estiver incluído)
find_package(raylib 5.0 REQUIRED)   # adapta-se à versão disponível

add_executable(arkanoid src/main.c)

target_link_libraries(arkanoid PRIVATE raylib)

# Incluir caminho para headers se for instalação não-padrão
# target_include_directories(arkanoid PRIVATE /caminho/do/raylib/include)
```

Compilação:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build    # gera build/arkanoid (ou arkanoid.exe)
```

---

## 4. Código-fonte (`src/main.c`)

> O programa abaixo tem \~200 linhas, suficientes para um Arkanoid funcional (paddle, bola, tijolos, placar e reinício). Sinta-se livre para expandir efeitos, sons e telas de menu.

```c
/********************************************************************
 * Arkanoid (Breakout) — versão mínima multiplataforma com raylib
 * Compila em C99.                                                   *
 ********************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_W   800
#define SCREEN_H   600

#define PADDLE_W   100
#define PADDLE_H    20
#define BALL_R      8
#define ROWS         5
#define COLS        10
#define BRICK_W    70
#define BRICK_H    20
#define BRICK_SP    4   // espaçamento

typedef struct {
    Rectangle rect;
    bool alive;
    Color color;
} Brick;

typedef struct {
    Vector2 pos, vel;
    float radius;
} Ball;

static void InitBricks(Brick bricks[ROWS][COLS])
{
    const int offsetX = (SCREEN_W - (COLS * (BRICK_W + BRICK_SP) - BRICK_SP)) / 2;
    const int offsetY = 60;

    for (int r = 0; r < ROWS; ++r)
    for (int c = 0; c < COLS; ++c) {
        bricks[r][c].rect = (Rectangle){
            offsetX + c * (BRICK_W + BRICK_SP),
            offsetY + r * (BRICK_H + BRICK_SP),
            BRICK_W, BRICK_H };
        bricks[r][c].alive = true;
        bricks[r][c].color = ColorFromHSV(r * 36.0f, 0.7f, 0.9f);
    }
}

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Arkanoid – Raylib");

    Rectangle paddle = {
        .x = (SCREEN_W - PADDLE_W) / 2.0f,
        .y = SCREEN_H - 40,
        .width  = PADDLE_W,
        .height = PADDLE_H
    };

    Ball ball = {
        .pos = { SCREEN_W / 2.0f, SCREEN_H / 2.0f },
        .vel = { 240, -240 },   // px/s
        .radius = BALL_R
    };

    Brick bricks[ROWS][COLS];
    InitBricks(bricks);

    int score = 0;
    bool gameOver = false;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        /* ---------- Lógica ---------- */

        // Reiniciar
        if (gameOver && IsKeyPressed(KEY_SPACE)) {
            ball.pos = (Vector2){ SCREEN_W/2.0f, SCREEN_H/2.0f };
            ball.vel = (Vector2){ 240, -240 };
            paddle.x = (SCREEN_W - PADDLE_W) / 2.0f;
            InitBricks(bricks);
            score = 0;
            gameOver = false;
        }

        // Movimento do paddle (teclado ou mouse)
        if (!gameOver) {
            if (IsKeyDown(KEY_LEFT))  paddle.x -= 450 * dt;
            if (IsKeyDown(KEY_RIGHT)) paddle.x += 450 * dt;
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                paddle.x = GetMouseX() - PADDLE_W / 2.0f;

            paddle.x = Clamp(paddle.x, 0, SCREEN_W - PADDLE_W);
        }

        // Movimento da bola
        if (!gameOver) {
            ball.pos.x += ball.vel.x * dt;
            ball.pos.y += ball.vel.y * dt;

            // colisões com bordas
            if (ball.pos.x <= ball.radius || ball.pos.x >= SCREEN_W - ball.radius)
                ball.vel.x *= -1.0f;
            if (ball.pos.y <= ball.radius)
                ball.vel.y *= -1.0f;
            if (ball.pos.y >= SCREEN_H + ball.radius)
                gameOver = true;

            // paddle
            if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle)) {
                ball.vel.y = -fabsf(ball.vel.y);            // sempre para cima
                float hit = (ball.pos.x - (paddle.x + PADDLE_W/2.0f)) / (PADDLE_W/2.0f);
                ball.vel.x = 300 * hit;
            }

            // tijolos
            for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
            {
                Brick *b = &bricks[r][c];
                if (!b->alive) continue;

                if (CheckCollisionCircleRec(ball.pos, ball.radius, b->rect)) {
                    b->alive = false;
                    score += 10;
                    ball.vel.y *= -1.0f;
                    goto skipRemaining;       // evita multi-colisão no mesmo frame
                }
            }
            skipRemaining: ;
        }

        /* ---------- Render ---------- */

        BeginDrawing();
            ClearBackground(BLACK);

            // Bricks
            for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (bricks[r][c].alive)
                    DrawRectangleRec(bricks[r][c].rect, bricks[r][c].color);

            // Paddle & Ball
            DrawRectangleRec(paddle, WHITE);
            DrawCircleV(ball.pos, ball.radius, YELLOW);

            // UI
            DrawText(TextFormat("SCORE: %05i", score), 10, 10, 20, RAYWHITE);
            if (gameOver)
                DrawText("GAME OVER  –  SPACE para reiniciar",
                         SCREEN_W/2 - MeasureText("GAME OVER  –  SPACE para reiniciar", 20)/2,
                         SCREEN_H/2 - 10, 20, RED);

            DrawFPS(SCREEN_W - 90, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

### Principais pontos técnicos

| Item                        | Explicação concisa                                                                                     |
| --------------------------- | ------------------------------------------------------------------------------------------------------ |
| **Estruturas**              | `Brick` encapsula retângulo, estado de vida e cor; `Ball` guarda posição, velocidade e raio.           |
| **CheckCollision • raylib** | Funções prontas (`CheckCollisionCircleRec`, `Clamp`) simplificam colisões, evitando física complexa.   |
| **Velocidade em px/s**      | Multiplicamos pelo `dt` (delta time) para manter movimento consistente em 30 Hz ou 144 Hz.             |
| **Rebote no paddle**        | Variamos `ball.vel.x` em função do ponto de impacto (`hit`), produzindo ângulos mais interessantes.    |
| **Goto**                    | Usado só para “quebrar” dois `for` aninhados após colisão; substitua por lógica diferente se preferir. |

---

## 5. Empacotamento multiplataforma

* **Linux/macOS** `cmake --build build --config Release && cp build/arkanoid ~/bin`
* **Windows**     `cmake --build build --config Release` gera `arkanoid.exe` na pasta correspondente.
* **Distribuição**

  * **Binário + DLL** (`raylib.dll` ou `.so`) num ZIP é suficiente.
  * Para um *bundle* 100 % estático, passe `-DBUILD_SHARED_LIBS=OFF` ao compilar o raylib.

---

## 6. Próximos passos

1. **Áudio** `InitAudioDevice()` + `PlaySound()` para efeitos de bounce.
2. **Menus & HUD** Estado `TITLE`, animações, power-ups (larger paddle, multi-ball).
3. **Persistência** Gravar *high-score* em arquivo binário (`SaveFileData`).
4. **Port Android/iOS** Use *raylib-game-template* ou *raylib-tech* e configure Gradle/CMake para gerar APK/IPA.

Com isso você tem um Arkanoid completamente funcional em cerca de 200 linhas, pronto para ser expandido e portar para qualquer plataforma principal sem alterar o código-fonte base. Bons estudos e boa diversão!
