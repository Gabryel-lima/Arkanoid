/********************************************************************
 * Arkanoid (Breakout) — versão mínima multiplataforma com raylib
 * Compila em C99.                                                   *
 ********************************************************************/

#include "raylib.h"
#include "defs.h"
#include "brick.h"
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

static void CreatePaddle(Rectangle* paddle) {
    paddle->x = (SCREEN_W - PADDLE_W) / 2.0f;
    paddle->y = SCREEN_H - 40;
    paddle->width  = PADDLE_W;
    paddle->height = PADDLE_H;
}

static void CreateBall(Ball* ball) {
    ball->pos = (Vector2) { SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    ball->vel = (Vector2) { GetRandomValue(-240, 240), -240 };   // px/s
    ball->radius = BALL_R;
}

static int ClampInt(int value, int min, int max)  {
    if (value < min ) return min;
    if (value > max ) return max;
    return value;
}

static void Reinit(Ball* ball, Rectangle* paddle, Brick bricks[ROWS][COLS], int *score, bool *gameOver) {
    PlaySound(restartSound);
    CreateBall(ball);
    CreatePaddle(paddle);
    InitBricks(bricks);
    *score = 0;
    *gameOver = false;
}

static void DrawBorders(void) {
    // Bordas visuais
    DrawRectangle(0, 0, SCREEN_W, 4, WHITE); // Topo
    DrawRectangle(0, SCREEN_H - 4, SCREEN_W, 4, WHITE); // Base
    DrawRectangle(0, 0, 4, SCREEN_H, WHITE); // Esquerda
    DrawRectangle(SCREEN_W - 4, 0, 4, SCREEN_H, WHITE); // Direita
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Arkanoid – Raylib");
    
    // Inicializar áudio
    InitAudioDevice();
    LoadSounds();

    // Paddle
    Rectangle paddle;
    CreatePaddle(&paddle);

    // Ball
    Ball ball;
    CreateBall(&ball);

    // Brick
    Brick bricks[ROWS][COLS];
    InitBricks(bricks);

    // States
    //bool Menu = false;
    int score = 0;
    bool gameOver = false;

    // Vsync
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ---------- Lógica ---------- */
        // Reiniciar
        if (gameOver && IsKeyPressed(KEY_SPACE)) {
            Reinit(&ball, &paddle, bricks, &score, &gameOver);
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
                float hit = (ball.pos.x - (paddle.x + PADDLE_W / 2.0f)) / (PADDLE_W / 2.0f);
                ball.vel.x = 300 * hit;
            }
            CreateBricks(bricks, &ball, &score);
        }

        /* ---------- Render ---------- */

        BeginDrawing();
            ClearBackground(BLACK);

            // Bordas visuais
            DrawBorders();

            // Bricks
            DrawBricks(bricks);

            // Paddle & Ball
            // DrawRectangleRec(paddle, WHITE);
            DrawRectangleRounded(paddle, 0.6f, 10, WHITE);
            DrawCircleV(ball.pos, ball.radius, YELLOW);

            // UI
            DrawText(TextFormat("SCORE: %05i", score), 10, 10, 20, RAYWHITE);
            if (gameOver)
                DrawText("GAME OVER  –  SPACE para reiniciar",
                    SCREEN_W / 2 - MeasureText("GAME OVER  –  SPACE para reiniciar", 20) / 2,
                    SCREEN_H / 2 - 10, 20, RED);

            DrawFPS(SCREEN_W - 90, 10);
        EndDrawing();
    }

    // Limpeza
    UnloadSounds();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
