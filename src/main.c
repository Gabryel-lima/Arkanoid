/********************************************************************
 * Arkanoid (Breakout) — versão mínima multiplataforma com raylib
 * Compila em C99.                                                   *
 ********************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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

// Sons do jogo
static Sound paddleHitSound;
static Sound brickHitSound;
static Sound gameOverSound;
static Sound restartSound;

typedef struct {
    Rectangle rect;
    bool alive;
    Color color;
} Brick;

typedef struct {
    Vector2 pos, vel;
    float radius;
} Ball;

// Função para carregar os sons
static void LoadSounds(void) {
    paddleHitSound = LoadSound("assets/sounds/paddle_hit.wav");
    brickHitSound = LoadSound("assets/sounds/brick_hit.wav");
    gameOverSound = LoadSound("assets/sounds/game_over.wav");
    restartSound = LoadSound("assets/sounds/restart.wav");
}

// Função para descarregar os sons
static void UnloadSounds(void) {
    UnloadSound(paddleHitSound);
    UnloadSound(brickHitSound);
    UnloadSound(gameOverSound);
    UnloadSound(restartSound);
}

static void InitBricks(Brick bricks[ROWS][COLS]) {
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

static int ClampInt(int value, int min, int max)  {
    if (value < min ) return min;
    if (value > max ) return max;
    return value;
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Arkanoid – Raylib");
    
    // Inicializar áudio
    InitAudioDevice();
    LoadSounds();

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
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ---------- Lógica ---------- */

        // Reiniciar
        if (gameOver && IsKeyPressed(KEY_SPACE)) {
            PlaySound(restartSound);
            ball.pos = (Vector2){ SCREEN_W / 2.0f, SCREEN_H / 2.0f };
            ball.vel = (Vector2){ GetRandomValue(-240, 240), -240 };
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

            paddle.x = ClampInt(paddle.x, 0, SCREEN_W - PADDLE_W);
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
            if (ball.pos.y >= SCREEN_H + ball.radius) {
                if (!gameOver) {
                    PlaySound(gameOverSound);
                }
                gameOver = true;
            }

            // paddle
            if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle)) {
                PlaySound(paddleHitSound);
                ball.vel.y = -fabsf(ball.vel.y);            // sempre para cima
                float hit = (ball.pos.x - (paddle.x + PADDLE_W/2.0f)) / (PADDLE_W/2.0f);
                ball.vel.x = 300 * hit;
            }

            // tijolos
            for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                Brick *b = &bricks[r][c];
                if (!b->alive) continue;

                if (CheckCollisionCircleRec(ball.pos, ball.radius, b->rect)) {
                    PlaySound(brickHitSound);
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

            // Bordas visuais
            DrawRectangle(0, 0, SCREEN_W, 4, WHITE); // Topo
            DrawRectangle(0, SCREEN_H - 4, SCREEN_W, 4, WHITE); // Base
            DrawRectangle(0, 0, 4, SCREEN_H, WHITE); // Esquerda
            DrawRectangle(SCREEN_W - 4, 0, 4, SCREEN_H, WHITE); // Direita

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

    // Limpeza
    UnloadSounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}