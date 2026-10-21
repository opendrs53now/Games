#include "paddle.h"
#include "raylib.h"
void PaddleInit(Paddle *p, float x, float y, float h){
    p->x=x; p->y=y; p->w=20; p->h=h; p->speed=850;
}
void PaddleUpdate(Paddle *p, int upKey, int downKey, float dt, int H){
    if(IsKeyDown(upKey)) p->y-=p->speed*dt;
    if(IsKeyDown(downKey)) p->y+=p->speed*dt;
    if(p->y<0) p->y=0;
    if(p->y>H-p->h) p->y=H-p->h;
}
void PaddleDraw(Paddle p){ DrawRectangle(p.x,p.y,p.w,p.h,WHITE); }
