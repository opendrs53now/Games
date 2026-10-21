#ifndef BALL_H
#define BALL_H
#include "raylib.h"
#define TRAIL_LENGTH 20
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Vector2 trail[TRAIL_LENGTH];
    int trailIndex;
} Ball;
void BallInit(Ball *b, int W, int H);
void ResetBall(Ball *b, int W, int H, float speedFactor);
void BallUpdate(Ball *b, float dt, int W, int H);
void BallDraw(Ball b);
#endif
