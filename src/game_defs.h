#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include "raylib.h"

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
    Vector2 pos, vel;
    float radius;
} Ball;

typedef struct {
    Rectangle rect;
    bool alive;
    Color color;
} Brick;

#endif // GAME_DEFS_H 