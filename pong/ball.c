#include "ball.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>
void BallInit(Ball *b, int W, int H) {
    b->radius = 12;
    b->trailIndex = 0;
    for (int i=0;i<TRAIL_LENGTH;i++) b->trail[i] = (Vector2){W/2.0f, H/2.0f};
    ResetBall(b, W, H, 1.0f);
}
void ResetBall(Ball *b, int W, int H, float speedFactor) {
    b->position = (Vector2){W/2.0f, H/2.0f};
    float sx = (rand()%2==0?1:-1) * 400 * speedFactor;
    float sy = (rand()%2==0?1:-1) * 300 * speedFactor;
    b->velocity = (Vector2){sx, sy};
    for (int i=0;i<TRAIL_LENGTH;i++) b->trail[i]=b->position;
}
void BallUpdate(Ball *b, float dt, int W, int H) {
    b->trail[b->trailIndex]=b->position;
    b->trailIndex=(b->trailIndex+1)%TRAIL_LENGTH;
    b->position.x += b->velocity.x*dt;
    b->position.y += b->velocity.y*dt;
    if (b->position.y < b->radius) { b->position.y=b->radius; b->velocity.y*=-1; }
    if (b->position.y > H-b->radius) { b->position.y=H-b->radius; b->velocity.y*=-1; }
}
void BallDraw(Ball b) {
    float hue = fmodf(GetTime()*40.0f, 360.0f);
    Color spec = ColorFromHSV(hue, 1.0f, 1.0f);
    for(int i=0;i<TRAIL_LENGTH;i++){
        int idx=(b.trailIndex+i)%TRAIL_LENGTH;
        float t=(float)i/TRAIL_LENGTH;
        DrawCircleV(b.trail[idx], b.radius*t, Fade(spec, t*0.5f));
    }
    DrawCircleV(b.position, b.radius, spec);
}
