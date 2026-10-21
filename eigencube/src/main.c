#include <raylib.h>
#include "rlgl.h"
#include <raymath.h>
#include <libguile.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define S 1.08f
#define CUBE 0.98f
typedef struct{ Color f[6]; int x,y,z; } Cubie;
Cubie cubies[3][3][3];
Color pal[6]={{220,0,0,255},{255,140,0,255},{255,255,255,255},{255,235,0,255},{0,180,60,255},{0,90,255,255}};
int animAxis=-1, animLayer=0; float animAngle=0,targetAngle=0; bool animating=false; bool g_won=false;
float camYaw=0.8f, camPitch=0.45f, camDist=9.0f; bool leftDown=false; Vector2 dragStart={0,0}; Cubie* dragCubie=NULL;
int moveCount=0; float gameTime=0; bool timerRunning=false;
char playerName[32]="Don"; bool nameEditing=false;
typedef struct{ char name[32]; int moves; float time; } Score;
Score top5[5];
void loadScores(){ FILE *f=fopen("scores.dat","rb"); if(f){ fread(top5,sizeof(top5),1,f); fclose(f); } else { strcpy(top5[0].name,"Don"); top5[0].moves=42; top5[0].time=38.2f; strcpy(top5[1].name,"Meta"); top5[1].moves=48; top5[1].time=41.5f; strcpy(top5[2].name,"Guest"); top5[2].moves=55; top5[2].time=52.1f; strcpy(top5[3].name,"---"); top5[3].moves=0; strcpy(top5[4].name,"---"); top5[4].moves=0; } }
void saveScores(){ FILE *f=fopen("scores.dat","wb"); if(f){ fwrite(top5,sizeof(top5),1,f); fclose(f); } }
void insertScore(){ if(moveCount==0) return; Score ns; strcpy(ns.name,playerName); ns.moves=moveCount; ns.time=gameTime; for(int i=0;i<5;i++){ if(top5[i].moves==0 || ns.moves < top5[i].moves || (ns.moves==top5[i].moves && ns.time < top5[i].time)){ for(int j=4;j>i;j--) top5[j]=top5[j-1]; top5[i]=ns; break; } } saveScores(); }
void initSolved(){ for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++) for(int z=-1;z<=1;z++){ Cubie *c=&cubies[x+1][y+1][z+1]; c->x=x; c->y=y; c->z=z; for(int k=0;k<6;k++) c->f[k]=(Color){0,0,0,0}; if(x==1) c->f[0]=pal[0]; if(x==-1) c->f[1]=pal[1]; if(y==1) c->f[2]=pal[2]; if(y==-1) c->f[3]=pal[3]; if(z==1) c->f[4]=pal[4]; if(z==-1) c->f[5]=pal[5]; } }
void finishRotation(){ Cubie tmp[3][3][3], newC[3][3][3]; for(int x=0;x<3;x++) for(int y=0;y<3;y++) for(int z=0;z<3;z++){ tmp[x][y][z]=cubies[x][y][z]; newC[x][y][z]=tmp[x][y][z]; } int dir=(targetAngle>0)?1:-1; for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++) for(int z=-1;z<=1;z++){ bool in=false; if(animAxis==0) in=(x==animLayer); if(animAxis==1) in=(y==animLayer); if(animAxis==2) in=(z==animLayer); if(!in) continue; int nx=x,ny=y,nz=z; if(animAxis==0){ ny=-dir*z; nz=dir*y; } else if(animAxis==1){ nx=dir*z; nz=-dir*x; } else { nx=-dir*y; ny=dir*x; } Cubie src=tmp[x+1][y+1][z+1], dst=src; if(animAxis==0){ if(dir==1){ dst.f[2]=src.f[5]; dst.f[4]=src.f[2]; dst.f[3]=src.f[4]; dst.f[5]=src.f[3]; } else { dst.f[2]=src.f[4]; dst.f[4]=src.f[3]; dst.f[3]=src.f[5]; dst.f[5]=src.f[2]; } }else if(animAxis==1){ if(dir==1){ dst.f[0]=src.f[4]; dst.f[5]=src.f[0]; dst.f[1]=src.f[5]; dst.f[4]=src.f[1]; } else { dst.f[0]=src.f[5]; dst.f[5]=src.f[1]; dst.f[1]=src.f[4]; dst.f[4]=src.f[0]; } }else{ if(dir==1){ dst.f[0]=src.f[3]; dst.f[2]=src.f[0]; dst.f[1]=src.f[2]; dst.f[3]=src.f[1]; } else { dst.f[0]=src.f[2]; dst.f[2]=src.f[1]; dst.f[1]=src.f[3]; dst.f[3]=src.f[0]; } } dst.x=nx; dst.y=ny; dst.z=nz; newC[nx+1][ny+1][nz+1]=dst; } for(int x=0;x<3;x++) for(int y=0;y<3;y++) for(int z=0;z<3;z++) cubies[x][y][z]=newC[x][y][z]; animating=false; animAxis=-1; animAngle=0; targetAngle=0; }
void doSync(int a,int l,int d){ animAxis=a; animLayer=l; targetAngle=d*90; finishRotation(); }
void scramble6(int m){ srand(time(NULL)); for(int i=0;i<m;i++){ int axis=rand()%3; int layer=(rand()%3)-1; int dir=(rand()%2)?1:-1; doSync(axis,layer,dir); } moveCount=0; gameTime=0; timerRunning=false; g_won=false; }
void startRot(int a,int l,int d){ if(animating) return; animAxis=a; animLayer=l; targetAngle=d*90; animAngle=0; animating=true; moveCount++; if(!timerRunning && !g_won) timerRunning=true; }
bool checkWin(){ for(int face=0;face<6;face++){ Color first={0,0,0,0}; bool got=false; for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++) for(int z=-1;z<=1;z++){ Cubie *c=&cubies[x+1][y+1][z+1]; if(c->f[face].a==0) continue; if(!got){ first=c->f[face]; got=true; } else if(c->f[face].r!=first.r||c->f[face].g!=first.g||c->f[face].b!=first.b) return false; } } return true; }
SCM scm_rotate(SCM s_axis, SCM s_layer, SCM s_dir){ int axis=0, layer=0, dir=1; if(scm_is_symbol(s_axis)){ SCM str=scm_symbol_to_string(s_axis); char *sym=scm_to_locale_string(str); if(sym[0]=='x'||sym[0]=='X') axis=0; else if(sym[0]=='y'||sym[0]=='Y') axis=1; else axis=2; free(sym); } else axis=scm_to_int(s_axis); layer=scm_to_int(s_layer); if(layer==1) layer=-1; else if(layer==2) layer=0; else if(layer==3) layer=1; dir=scm_to_int(s_dir); if(!animating) startRot(axis,layer,dir); return SCM_BOOL_T; }
SCM scm_scramble(SCM s_n){ scramble6(scm_to_int(s_n)); return SCM_BOOL_T; }
SCM scm_solve(){ initSolved(); moveCount=0; gameTime=0; timerRunning=false; return SCM_BOOL_T; }
SCM scm_winp(){ return g_won?SCM_BOOL_T:SCM_BOOL_F; }
int closestAxis(Vector3 v){ float ax=fabsf(v.x),ay=fabsf(v.y),az=fabsf(v.z); if(ax>=ay && ax>=az) return 0; if(ay>=ax && ay>=az) return 1; return 2; }
int main(void){
 scm_init_guile(); scm_c_define_gsubr("rotate",3,0,0,(void*)scm_rotate); scm_c_define_gsubr("scramble",1,0,0,(void*)scm_scramble); scm_c_define_gsubr("solve",0,0,0,(void*)scm_solve); scm_c_define_gsubr("win?",0,0,0,(void*)scm_winp);
 InitWindow(1600,1024,"Eigencube v8.5.1"); SetTargetFPS(60); loadScores(); initSolved(); scramble6(40); scm_c_primitive_load("scheme/live.scm"); float t=0;
 Rectangle btnReady={20, 1024-88, 160, 36}; Rectangle btnEnd={200, 1024-88, 140, 36};
 while(!WindowShouldClose()){
  scm_c_primitive_load("scheme/live.scm"); SCM var=scm_c_lookup("pending-move"); SCM val=SCM_BOOL_F; if(scm_is_true(var)) val=scm_variable_ref(var);
  if(scm_is_true(val) && scm_is_true(scm_list_p(val))){ if(scm_to_int(scm_length(val))==3){ SCM a=scm_list_ref(val,scm_from_int(0)); SCM l=scm_list_ref(val,scm_from_int(1)); SCM d=scm_list_ref(val,scm_from_int(2)); scm_rotate(a,l,d); scm_c_eval_string("(set! pending-move #f)"); } }
  float dt=GetFrameTime(); t+=dt; if(timerRunning && !g_won && !nameEditing) gameTime+=dt;
  if(nameEditing){ int key=GetCharPressed(); while(key>0){ if(key>=32 && key<=125 && strlen(playerName)<28){ int len=strlen(playerName); playerName[len]=(char)key; playerName[len+1]=0; } key=GetCharPressed(); } if(IsKeyPressed(KEY_BACKSPACE) && strlen(playerName)>0) playerName[strlen(playerName)-1]=0; if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) nameEditing=false; } else { if(IsKeyPressed(KEY_N)) nameEditing=true; }
  Vector2 mp=GetMousePosition(); bool hoverReady=CheckCollisionPointRec(mp, btnReady); bool hoverEnd=CheckCollisionPointRec(mp, btnEnd);
  if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
   if(hoverReady){ scramble6(50); }
   else if(hoverEnd){ timerRunning=false; if(!g_won && moveCount>0) insertScore(); }
   else if(mp.y>1024-90 && mp.x>860 && mp.x<1080 && !nameEditing) nameEditing=true;
  }
  if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){ Vector2 md=GetMouseDelta(); camYaw+=md.x*0.008f; camPitch+=md.y*0.008f; if(camPitch>1.5f) camPitch=1.5f; if(camPitch<-1.5f) camPitch=-1.5f; }
  camDist-=GetMouseWheelMove()*0.5f; if(camDist<5) camDist=5; if(camDist>18) camDist=18;
  Camera cam={0}; cam.fovy=32; cam.up=(Vector3){0,1,0}; cam.target=(Vector3){0,0,0}; cam.projection=CAMERA_PERSPECTIVE; cam.position.x=cosf(camYaw)*cosf(camPitch)*camDist; cam.position.y=sinf(camPitch)*camDist; cam.position.z=sinf(camYaw)*cosf(camPitch)*camDist;
  Vector3 fwd=Vector3Normalize((Vector3){-cam.position.x,-cam.position.y,-cam.position.z}); Vector3 rgt=Vector3Normalize(Vector3CrossProduct(fwd,(Vector3){0,1,0})); if(Vector3Length(rgt)<0.1f) rgt=(Vector3){1,0,0}; Vector3 up=Vector3CrossProduct(rgt,fwd);
  int upAxis=closestAxis(up); int rightAxis=closestAxis(rgt);
  float dotX=fabsf(fwd.x), dotY=fabsf(fwd.y), dotZ=fabsf(fwd.z); int domFace=0; if(dotY>dotX && dotY>dotZ) domFace=1; else if(dotZ>dotX && dotZ>dotY) domFace=2;
  if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !animating && !nameEditing && !hoverReady && !hoverEnd){
   Ray ray=GetMouseRay(GetMousePosition(),cam); float best=9999; Cubie* hit=NULL; for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++) for(int z=-1;z<=1;z++){ Cubie *cb=&cubies[x+1][y+1][z+1]; bool isPoleCenter=(abs(cb->x)+abs(cb->y)+abs(cb->z)==1); if(isPoleCenter) continue; Vector3 pos={(float)cb->x*S,(float)cb->y*S,(float)cb->z*S + sinf(t*0.6f)*0.08f}; BoundingBox box={{pos.x-CUBE/2,pos.y-CUBE/2,pos.z-CUBE/2},{pos.x+CUBE/2,pos.y+CUBE/2,pos.z+CUBE/2}}; RayCollision c=GetRayCollisionBox(ray,box); if(c.hit && c.distance<best){ best=c.distance; hit=cb; } } if(hit){ leftDown=true; dragStart=GetMousePosition(); dragCubie=hit; } }
  if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && leftDown && !animating && dragCubie && !nameEditing){
   Vector2 cur=GetMousePosition(); Vector2 D={cur.x-dragStart.x, cur.y-dragStart.y}; if(Vector2Length(D)>18){ int chosenAxis,chosenLayer; int isHorizontal=(fabsf(D.x)>fabsf(D.y))?1:0; if(domFace==1){ if(isHorizontal){ chosenAxis=2; chosenLayer=dragCubie->z; } else { chosenAxis=0; chosenLayer=dragCubie->x; } } else{ if(isHorizontal){ chosenAxis=upAxis; if(chosenAxis==0) chosenLayer=dragCubie->x; else if(chosenAxis==1) chosenLayer=dragCubie->y; else chosenLayer=dragCubie->z; } else { chosenAxis=rightAxis; if(chosenAxis==0) chosenLayer=dragCubie->x; else if(chosenAxis==1) chosenLayer=dragCubie->y; else chosenLayer=dragCubie->z; } } Vector3 b={(float)dragCubie->x*S,(float)dragCubie->y*S,(float)dragCubie->z*S}; Vector3 bTest=b; if(chosenAxis==0){ if(fabsf(bTest.y)<0.01f && fabsf(bTest.z)<0.01f) bTest.y=0.5f; } else if(chosenAxis==1){ if(fabsf(bTest.x)<0.01f && fabsf(bTest.z)<0.01f) bTest.x=0.5f; } else { if(fabsf(bTest.x)<0.01f && fabsf(bTest.y)<0.01f) bTest.x=0.5f; } Vector3 posPlus=bTest, posMinus=bTest; if(chosenAxis==0){ posPlus=(Vector3){bTest.x, -bTest.z, bTest.y}; posMinus=(Vector3){bTest.x, bTest.z, -bTest.y}; } else if(chosenAxis==1){ posPlus=(Vector3){bTest.z, bTest.y, -bTest.x}; posMinus=(Vector3){-bTest.z, bTest.y, bTest.x}; } else { posPlus=(Vector3){-bTest.y, bTest.x, bTest.z}; posMinus=(Vector3){bTest.y, -bTest.x, bTest.z}; } Vector2 s0=GetWorldToScreen(bTest,cam); Vector2 sP=GetWorldToScreen(posPlus,cam); Vector2 sM=GetWorldToScreen(posMinus,cam); Vector2 vP={sP.x-s0.x, sP.y-s0.y}; Vector2 vM={sM.x-s0.x, sM.y-s0.y}; float dotP=vP.x*D.x+vP.y*D.y; float dotM=vM.x*D.x+vM.y*D.y; int chosenDir=(dotP>=dotM)?1:-1; startRot(chosenAxis,chosenLayer,chosenDir); } leftDown=false; dragCubie=NULL; }
  if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) leftDown=false;
  if(!animating){ if(IsKeyPressed(KEY_R) && !nameEditing) scramble6(40); if(IsKeyPressed(KEY_T) && !nameEditing){ initSolved(); timerRunning=false; } }
  if(animating){ animAngle+=(targetAngle>0?360:-360)*GetFrameTime(); if(fabsf(animAngle)>=fabsf(targetAngle)){ finishRotation(); if(checkWin() && !g_won){ g_won=true; timerRunning=false; insertScore(); } } }
  BeginDrawing(); ClearBackground((Color){14,14,18,255}); DrawRectangle(0,0,1600,48,Fade(BLACK,0.9f)); DrawText("Eigencube 54 | L-drag Row/Col | R-drag Orbit | R:Scramble T:Solve N:Name", 20, 12, 20, RAYWHITE); DrawFPS(1500,12); BeginMode3D(cam); rlPushMatrix(); rlTranslatef(0,sinf(t*0.6f)*0.08f,0); for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++) for(int z=-1;z<=1;z++){ Cubie *cb=&cubies[x+1][y+1][z+1]; Vector3 pos={(float)cb->x*S,(float)cb->y*S,(float)cb->z*S}; bool isPoleCenter=(abs(cb->x)+abs(cb->y)+abs(cb->z)==1); bool isDragged=(dragCubie && cb->x==dragCubie->x && cb->y==dragCubie->y && cb->z==dragCubie->z && leftDown); bool inRot=animating && ((animAxis==0&&cb->x==animLayer)||(animAxis==1&&cb->y==animLayer)||(animAxis==2&&cb->z==animLayer)); rlPushMatrix(); if(inRot){ if(animAxis==0) rlRotatef(animAngle,1,0,0); else if(animAxis==1) rlRotatef(animAngle,0,1,0); else rlRotatef(animAngle,0,0,1); } Color baseC=isPoleCenter?(Color){50,50,55,255}:(isDragged?(Color){90,90,100,255}:(Color){30,30,34,255}); DrawCube(pos,CUBE,CUBE,CUBE,baseC); if(cb->f[0].a){ Vector3 p={pos.x+0.505f,pos.y,pos.z}; DrawCube(p,0.02f,0.92f,0.92f,cb->f[0]); } if(cb->f[1].a){ Vector3 p={pos.x-0.505f,pos.y,pos.z}; DrawCube(p,0.02f,0.92f,0.92f,cb->f[1]); } if(cb->f[2].a){ Vector3 p={pos.x,pos.y+0.505f,pos.z}; DrawCube(p,0.92f,0.02f,0.92f,cb->f[2]); } if(cb->f[3].a){ Vector3 p={pos.x,pos.y-0.505f,pos.z}; DrawCube(p,0.92f,0.02f,0.92f,cb->f[3]); } if(cb->f[4].a){ Vector3 p={pos.x,pos.y,pos.z+0.505f}; DrawCube(p,0.92f,0.92f,0.02f,cb->f[4]); } if(cb->f[5].a){ Vector3 p={pos.x,pos.y,pos.z-0.505f}; DrawCube(p,0.92f,0.92f,0.02f,cb->f[5]); } rlPopMatrix(); } rlPopMatrix(); EndMode3D();
   DrawRectangle(0,1024-96,1600,96,Fade(BLACK,0.92f));
   DrawRectangleRec(btnReady, hoverReady?DARKGREEN:GREEN); DrawText("READY TO PLAY", btnReady.x+12, btnReady.y+10, 14, WHITE);
   DrawRectangleRec(btnEnd, hoverEnd?MAROON:RED); DrawText("END GAME", btnEnd.x+20, btnEnd.y+10, 14, WHITE);
   char timeBuf[64]; if(timerRunning) snprintf(timeBuf,sizeof(timeBuf),"Time %02d:%05.2f", (int)(gameTime/60), fmodf(gameTime,60)); else snprintf(timeBuf,sizeof(timeBuf),"Time %02d:%05.2f (READY)", (int)(gameTime/60), fmodf(gameTime,60)); DrawText(timeBuf, 380, 1024-62, 22, timerRunning?SKYBLUE:GRAY);
   char movesBuf[64]; snprintf(movesBuf,sizeof(movesBuf),"Moves %d", moveCount); DrawText(movesBuf, 660, 1024-62, 22, YELLOW);
   if(g_won) DrawText("SOLVED!", 800, 1024-64, 26, GREEN);
   DrawText("TOP 5:", 860, 1024-80, 18, RAYWHITE);
   for(int i=0;i<3;i++){ char rb[96]; if(top5[i].moves==0) snprintf(rb,sizeof(rb),"%d. ---", i+1); else snprintf(rb,sizeof(rb),"%d. %s %dm %02d:%04.1fs", i+1, top5[i].name, top5[i].moves, (int)(top5[i].time/60), fmodf(top5[i].time,60)); DrawText(rb, 860, 1024-56 + i*16, 14, i==0?GOLD:RAYWHITE); }
   for(int i=3;i<5;i++){ char rb[96]; if(top5[i].moves==0) snprintf(rb,sizeof(rb),"%d. ---", i+1); else snprintf(rb,sizeof(rb),"%d. %s %dm %02d:%04.1fs", i+1, top5[i].name, top5[i].moves, (int)(top5[i].time/60), fmodf(top5[i].time,60)); DrawText(rb, 1120, 1024-56 + (i-3)*16, 14, RAYWHITE); }
   if(nameEditing){ DrawRectangle(1320,1024-68,260,44,Fade(DARKBLUE,0.95f)); DrawText(TextFormat("Player: %s_", playerName), 1330, 1024-56, 18, YELLOW); } else { DrawText(TextFormat("Player: %s [N]", playerName), 1330, 1024-56, 18, LIGHTGRAY); }
   if(leftDown) DrawLineEx(dragStart,GetMousePosition(),4,Fade(YELLOW,0.8f));
  EndDrawing();
 }
 CloseWindow(); return 0;
}
