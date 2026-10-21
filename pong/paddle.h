#ifndef PADDLE_H
#define PADDLE_H
typedef struct { float x,y,w,h,speed; } Paddle;
void PaddleInit(Paddle *p, float x, float y, float h);
void PaddleUpdate(Paddle *p, int upKey, int downKey, float dt, int H);
void PaddleDraw(Paddle p);
#endif
