/********************************************************************
 * Arkanoid (Breakout) — versão mínima multiplataforma com raylib
 * Compila em C99.                                                   *
 ********************************************************************/

#include "raylib.h"
#include "game_defs.h"
#include "bot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

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

static void Reinit(Ball* ball, Rectangle* paddle, Brick bricks[ROWS][COLS], int *score, bool *gameOver) {
    PlaySound(restartSound);
    CreateBall(ball);
    CreatePaddle(paddle);
    InitBricks(bricks);
    *score = 0;
    *gameOver = false;
}

typedef enum { MENU, JOGO, JOGO_BOT, JOGO_BOT_SEGUIDOR } GameMode;

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
    int score = 0;
    bool gameOver = false;

    // Q-Learning (apenas para o modo bot)
    float **Q = NULL;
    float alpha = 0.1f;
    float gamma = 0.99f;
    float epsilon = 0.1f;
    int last_state = -1, last_action = -1;

    // Vsync
    SetTargetFPS(60);
    GameMode mode = MENU;
    int menuOption = 0; // 0 = Jogador, 1 = Bot Q-Learning, 2 = Bot Seguidor
    const char* menuItems[3] = {
        "Jogar Manualmente",
        "Modo Bot (Q-Learning)",
        "Modo Bot (Seguidor)"
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Inicializa Q-table ao entrar no modo bot
        if (mode == JOGO_BOT && Q == NULL) {
            Q = init_q_table();
            last_state = -1;
            last_action = -1;
            printf("[DEBUG] Q-table inicializada.\n");
        }
        // Libera Q-table ao sair do modo bot
        if (mode != JOGO_BOT && Q != NULL) {
            for (int i = 0; i < N_STATES; i++) free(Q[i]);
            free(Q); Q = NULL;
            printf("[DEBUG] Q-table liberada.\n");
        }

        if (mode == MENU) {
            // Navegação do menu
            if (IsKeyPressed(KEY_UP))   menuOption = (menuOption + 2) % 3;
            if (IsKeyPressed(KEY_DOWN)) menuOption = (menuOption + 1) % 3;
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                if (menuOption == 0) mode = JOGO;
                else if (menuOption == 1) mode = JOGO_BOT;
                else if (menuOption == 2) mode = JOGO_BOT_SEGUIDOR;
                Reinit(&ball, &paddle, bricks, &score, &gameOver);
            }
            // Render do menu
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("ARKANOID", SCREEN_W / 2 - MeasureText("ARKANOID", 40) / 2, 100, 40, YELLOW);
                for (int i = 0; i < 3; i++) {
                    Color color = (i == menuOption) ? RED : RAYWHITE;
                    DrawText(menuItems[i], SCREEN_W / 2 - MeasureText(menuItems[i], 24) / 2, 220 + i*40, 24, color);
                }
                DrawText("Use ↑/↓ para escolher e ENTER/ESPAÇO para confirmar", SCREEN_W / 2 - 220, 350, 18, GRAY);
            EndDrawing();
            continue;
        }

        /* ---------- Lógica ---------- */
        // Reiniciar
        if (gameOver && IsKeyPressed(KEY_SPACE)) {
            Reinit(&ball, &paddle, bricks, &score, &gameOver);
        }

        // --- Reinício automático no modo bot ---
        if (mode == JOGO_BOT && gameOver) {
            Reinit(&ball, &paddle, bricks, &score, &gameOver);
            last_state = -1;
            last_action = -1;
        }

        // Movimento do paddle (teclado ou mouse ou bot)
        if (!gameOver) {
            if (mode == JOGO) {
                if (IsKeyDown(KEY_LEFT))  paddle.x -= 450 * dt;
                if (IsKeyDown(KEY_RIGHT)) paddle.x += 450 * dt;
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    paddle.x = GetMouseX() - PADDLE_W / 2.0f;
                paddle.x = ClampInt(paddle.x, 0, SCREEN_W - PADDLE_W);
            } else if (mode == JOGO_BOT) {
                int state = encode_state(paddle, ball);
                assert(state >= 0 && state < N_STATES);
                int action;
                if (((float)rand() / RAND_MAX) < epsilon) {
                    action = GetRandomValue(0, N_ACTIONS-1); // Exploração
                } else {
                    float maxQ = Q[state][0];
                    int bestActions[N_ACTIONS], nBest = 1;
                    bestActions[0] = 0;
                    for (int a = 1; a < N_ACTIONS; a++) {
                        if (Q[state][a] > maxQ) {
                            maxQ = Q[state][a];
                            bestActions[0] = a;
                            nBest = 1;
                        } else if (Q[state][a] == maxQ) {
                            bestActions[nBest++] = a;
                        }
                    }
                    action = bestActions[GetRandomValue(0, nBest-1)];
                }
                float move = 0;
                if (action == 0) move = -450 * dt; // Esquerda
                else if (action == 2) move =  450 * dt; // Direita
                paddle.x += move;
                paddle.x = ClampInt(paddle.x, 0, SCREEN_W - PADDLE_W);

                if (last_state != -1 && last_action != -1) {
                    float reward = 0.0f;
                    if (gameOver) reward = -1.0f;
                    else if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle)) reward = 1.0f;
                    int next_state = state;
                    assert(last_state >= 0 && last_state < N_STATES);
                    q_learning_update(Q, last_state, last_action, reward, next_state, alpha, gamma, N_ACTIONS);
                }
                last_state = state;
                last_action = action;
            } else if (mode == JOGO_BOT_SEGUIDOR) {
                if (ball.pos.x < paddle.x + PADDLE_W / 2) paddle.x -= 450 * dt;
                else if (ball.pos.x > paddle.x + PADDLE_W / 2) paddle.x += 450 * dt;
                paddle.x = ClampInt(paddle.x, 0, SCREEN_W - PADDLE_W);
            }
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

        // tijolos
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                Brick *b = &bricks[r][c];
                if (!b->alive) continue;

                if (CheckCollisionCircleRec(ball.pos, ball.radius, b->rect)) {
                    PlaySound(brickHitSound);
                    b->alive = false;
                    score += 10;

                    // Calcula as distâncias das bordas da bola em relação ao bloco
                    float dist_top    = fabsf((ball.pos.y + ball.radius) - b->rect.y);
                    float dist_bottom = fabsf((ball.pos.y - ball.radius) - (b->rect.y + b->rect.height));
                    float dist_left   = fabsf((ball.pos.x + ball.radius) - b->rect.x);
                    float dist_right  = fabsf((ball.pos.x - ball.radius) - (b->rect.x + b->rect.width));

                    // Verifica se a colisão é mais próxima dos lados vertical ou horizontal
                    if (fminf(dist_top, dist_bottom) < fminf(dist_left, dist_right)) {
                        // Colisão no topo ou na base do bloco, inverte o eixo Y
                        ball.vel.y *= -1.0f;
                    } else {
                        // Colisão nos lados esquerdo ou direito do bloco, inverte o eixo X
                        ball.vel.x *= -1.0f;
                    }

                    goto skipRemaining; // evita multi-colisão no mesmo frame
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