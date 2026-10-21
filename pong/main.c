#include "raylib.h" // Include raylib core: window, drawing, input, timing
#include "paddle.h" // Include our paddle module: Paddle struct + PaddleInit/Update/Draw
#include "ball.h" // Include our ball module: Ball struct + BallInit/Update/Draw/ResetBall
#include "score.h" // Include our score module: ScoreDraw()
#include <string.h> // Include string.h for strlen(), strcpy(), strcmp()
#include <math.h> // Include math.h for fmodf(), atan2f(), cosf(), sinf(), ceilf()

const int W = 1820, H = 980; // Define window width and height as global constants

int main() { // Program entry point
    InitWindow(W, H, "Pong Modular"); // Create raylib window W x H with title
    SetTargetFPS(60); // Lock game loop to 60 frames per second

    char n1[16]="Player 1", n2[16]="Player 2"; // Buffers for player names, default values
    int target=5, bestOf=3, difficulty=0, stage=0; // target points per game, best-of series, difficulty 0=unset, stage 0-5 menu flow
    int s1=0, s2=0, g1=0, g2=0; // s1/s2 = points this game, g1/g2 = games won this series
    float paddleH=110, ballSF=1.0f; // paddleH = paddle height, ballSF = ball speed multiplier
    bool seriesOver=false; // Flag true when someone has won the series
    float countdown=5.0f; // 5-second get-ready countdown before play starts
    float varTimer=0.0f; // Timer for difficulty 4: tracks time since last speed change

    Paddle p1, p2; Ball ball; // Declare two paddles and one ball structs
    PaddleInit(&p1, 20, H/2-paddleH/2, paddleH); // Init left paddle at x=20, vertically centered
    PaddleInit(&p2, W-40, H/2-paddleH/2, paddleH); // Init right paddle at x=W-40, vertically centered
    BallInit(&ball, W, H); // Init ball in center with default velocity

    while (!WindowShouldClose()) { // Main game loop, runs until X clicked or ESC
        float dt = GetFrameTime(); // dt = seconds since last frame, for frame-rate independent movement

        float hue = fmodf(GetTime()*40.0f, 360.0f); // hue cycles 0-360 over time for rainbow effect
        Color spec = ColorFromHSV(hue, 1.0f, 1.0f); // Convert HSV hue to RGB Color for menu titles

        if (stage < 5) { // If we are still in menus (0=name1,1=name2,2=target,3=bestOf,4=difficulty)
            int k = GetCharPressed(); // Get last Unicode char pressed this frame, 0 if none
            if (k>0) { // If a character was pressed
                if (stage==0 && strlen(n1)<15 && k>=32 && k<127) { // Stage 0: typing name 1, printable ASCII, room left
                    if (strcmp(n1,"Player 1")==0) n1[0]=0; // If still default, clear it on first keystroke
                    int l=strlen(n1); n1[l]=k; n1[l+1]=0; // Append char k and null-terminate
                }
                if (stage==1 && strlen(n2)<15 && k>=32 && k<127) { // Stage 1: same logic for player 2
                    if (strcmp(n2,"Player 2")==0) n2[0]=0; // Clear default "Player 2"
                    int l=strlen(n2); n2[l]=k; n2[l+1]=0; // Append char
                }
                if (stage==2 && k>='0' && k<='9' && target<99) target = target*10 + (k-'0'); // Stage 2: build target number digit by digit
                if (stage==3 && (k=='1'||k=='3'||k=='5'||k=='7')) bestOf = k-'0'; // Stage 3: only allow 1,3,5,7
                if (stage==4 && k>='1' && k<='4') { // Stage 4: difficulty 1-4 pressed
                    difficulty=k-'0'; // Convert char to int 1-4
                    float hs[4]={135, 108, 80, 110}; // Paddle heights per difficulty
                    float sfs[4]={0.875f, 1.44f, 2.0f, 1.0f}; // Ball speed factors per difficulty
                    paddleH=hs[difficulty-1]; ballSF=sfs[difficulty-1]; // Look up chosen values
                    PaddleInit(&p1,20,H/2-paddleH/2,paddleH); // Re-init p1 with new height
                    PaddleInit(&p2,W-40,H/2-paddleH/2,paddleH); // Re-init p2 with new height
                    float ps = 600 * (0.8f + 0.4f*ballSF); // Paddle speed scales with ball speed: 0.5x->600, 2.0x->960
                    p1.speed = ps; p2.speed = ps; // Apply paddle speed to both
                    ResetBall(&ball,W,H,ballSF); // Reset ball with new speed factor
                    stage=5; s1=s2=g1=g2=0; seriesOver=false; countdown=5.0f; varTimer=0.0f; // Jump to game, reset scores/timers
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE)) { // Handle backspace key
                if(stage==0 && strlen(n1)>0) n1[strlen(n1)-1]=0; // Delete last char of n1
                if(stage==1 && strlen(n2)>0) n2[strlen(n2)-1]=0; // Delete last char of n2
                if(stage==2) target/=10; // Delete last digit of target (e.g. 12 -> 1)
            }
            if (IsKeyPressed(KEY_ENTER)) { // Handle ENTER to advance menu stage
                if(stage==0){ if(strlen(n1)==0) strcpy(n1,"Player 1"); stage++; } // Confirm p1 name, default if empty
                else if(stage==1){ if(strlen(n2)==0) strcpy(n2,"Player 2"); stage++; } // Confirm p2 name
                else if(stage==2){ if(target<=0) target=5; stage++; } // Confirm target, default 5
                else if(stage==3){ stage++; } // Confirm bestOf, go to difficulty
                else if(stage==4 && difficulty==0){ // If ENTER pressed without picking difficulty, use default
                    difficulty=2; paddleH=110; ballSF=1.25f; // Default = medium
                    PaddleInit(&p1,20,H/2-paddleH/2,paddleH); // Re-init paddles
                    PaddleInit(&p2,W-40,H/2-paddleH/2,paddleH);
                    float ps = 600 * (0.8f + 0.4f*ballSF); // Calc paddle speed
                    p1.speed = ps; p2.speed = ps; // Apply it
                    ResetBall(&ball,W,H,ballSF); // Reset ball
                    stage=5; s1=s2=g1=g2=0; seriesOver=false; countdown=5.0f; varTimer=0.0f; // Start game
                }
            }
            BeginDrawing(); ClearBackground(BLACK); // Start frame, clear to black
            if(stage==0){ // Draw player 1 name entry screen
                DrawText("Enter Player 1 name, then ENTER", W/2-350, H/2-80, 32, spec); // Title in rainbow
                DrawText("(default: Player 1)", W/2-350, H/2-40, 24, GRAY); // Hint
                DrawText(TextFormat("%s_", n1), W/2-350, H/2+20, 36, WHITE); // Show typed name + cursor
            }
            if(stage==1){ // Draw player 2 name entry screen
                DrawText("Enter Player 2 name, then ENTER", W/2-350, H/2-80, 32, spec);
                DrawText("(default: Player 2)", W/2-350, H/2-40, 24, GRAY);
                DrawText(TextFormat("%s_", n2), W/2-350, H/2+20, 36, WHITE);
            }
            if(stage==2){ // Draw target points screen
                DrawText("Points to win a game:", W/2-350, H/2-80, 32, spec);
                DrawText(TextFormat("%d", target), W/2-350, H/2-20, 40, WHITE); // Show current number
                DrawText("type number, then ENTER", W/2-350, H/2+40, 24, GRAY);
            }
            if(stage==3){ // Draw best-of screen
                DrawText("Best of how many games?", W/2-350, H/2-80, 32, spec);
                DrawText(TextFormat("1 3 5 7 (current: %d)", bestOf), W/2-350, H/2-20, 30, WHITE);
                DrawText("press 1/3/5/7, then ENTER", W/2-350, H/2+40, 24, GRAY);
            }
            if(stage==4){ // Draw difficulty select screen
                DrawText("Select difficulty:", W/2-350, H/2-120, 32, spec);
                DrawText("1 SLOW (0.88x) + big paddle", W/2-350, H/2-60, 28, WHITE);
                DrawText("2 MEDIUM (1.44x)", W/2-350, H/2-20, 28, WHITE);
                DrawText("3 FAST (2.0x)", W/2-350, H/2+20, 28, WHITE);
                DrawText("4 VARIABLE (0.5x-2.0x random per hit)", W/2-350, H/2+60, 28, WHITE);
            }
            EndDrawing(); // Finish frame
            continue; // Skip rest of loop, stay in menu
        }

        if (seriesOver) { // If series is over, show winner screen
            BeginDrawing(); ClearBackground(BLACK); // Start frame
            DrawText(TextFormat("%s WINS SERIES %d-%d!", g1>g2?n1:n2, g1, g2), W/2-450, H/2-60, 48, spec); // Winner text
            DrawText("[Y] Play again [N] End", W/2-300, H/2+30, 36, WHITE); // Options
            EndDrawing(); // End frame
            if (IsKeyPressed(KEY_Y)) { // Y = restart
                s1=s2=g1=g2=0; seriesOver=false; stage=0; target=5; bestOf=3; difficulty=0; // Reset all
                strcpy(n1,"Player 1"); strcpy(n2,"Player 2"); // Reset names
            }
            if (IsKeyPressed(KEY_N)) break; // N = break loop, quit
            continue; // Skip gameplay
        }

        if (countdown > 0) { // 5-second pre-game countdown
            countdown -= dt; // Decrease timer by frame time
            BeginDrawing(); // Start frame
            ClearBackground(BLACK); // Clear
            PaddleDraw(p1); PaddleDraw(p2); BallDraw(ball); // Draw field behind text
            ScoreDraw(s1,s2,n1,n2,W,target); // Draw score header
            DrawText(TextFormat("Games: %d - %d (Best of %d)", g1, g2, bestOf), W/2-160, 95, 26, GRAY); // Series score
            DrawText(TextFormat("Get ready: %d", (int)ceilf(countdown)), W/2-120, H/2-40, 60, YELLOW); // Big countdown number
            DrawText("W/S vs P/L - fingers on keys!", W/2-300, H/2+40, 28, GRAY); // Controls hint
            EndDrawing(); // End frame
            continue; // Don't update gameplay yet
        }

        PaddleUpdate(&p1, KEY_W, KEY_S, dt, H); // Move left paddle with W/S, clamp to screen
        PaddleUpdate(&p2, KEY_P, KEY_L, dt, H); // Move right paddle with Up/Down
        BallUpdate(&ball, dt, W, H); // Move ball, bounce off top/bottom walls

        static bool wasOnWall = false; // Remember if ball was touching wall last frame (for edge detection)
        bool onWall = (ball.position.y - ball.radius <= 1 || ball.position.y + ball.radius >= H-1); // True if touching top/bottom now

        if (difficulty==4 && onWall &&!wasOnWall) { // Difficulty 4: on new wall hit (not continuous contact)
            float v = 0.5f + (float)GetRandomValue(0,150)/100.0f; // Random speed 0.50 to 2.00
            float ang = atan2f(ball.velocity.y, ball.velocity.x); // Get current travel angle
            float ns = 400 * v; // New speed = base 400 * factor
            ball.velocity.x = cosf(ang)*ns; ball.velocity.y = sinf(ang)*ns; // Keep direction, change magnitude
            ballSF = v; // Store factor for HUD and ResetBall
            float ps2 = 600 * (0.8f + 0.4f*ballSF); // Scale paddle speed with ball speed
            p1.speed = ps2; p2.speed = ps2; // Apply to both paddles
            varTimer = 0.0f; // Reset slow-ball timer because we just changed speed
        }
        wasOnWall = onWall; // Update edge detector for next frame

        // 2-second timeout re-roll if stuck slow
        if (difficulty==4) { // Only in variable mode
            varTimer += dt; // Accumulate time since last change
            if (varTimer > 2.0f && ballSF < 0.9f) { // If >2 sec and still slow (<0.9x)
                float v = 0.5f + (float)GetRandomValue(0,150)/100.0f; // Pick new random speed
                float ang = atan2f(ball.velocity.y, ball.velocity.x); // Keep direction
                float ns = 400 * v; // Compute new speed
                ball.velocity.x = cosf(ang)*ns; ball.velocity.y = sinf(ang)*ns; // Apply
                ballSF = v; // Store
                float ps2 = 600 * (0.8f + 0.4f*ballSF); // Rescale paddles
                p1.speed = ps2; p2.speed = ps2;
                varTimer = 0.0f; // Reset timer
            }
        }

        if (ball.position.x < 0) { s2++; ResetBall(&ball,W,H,ballSF); varTimer=0.0f; } // Ball went off left, point for p2
        if (ball.position.x > W) { s1++; ResetBall(&ball,W,H,ballSF); varTimer=0.0f; } // Ball went off right, point for p1

        if (CheckCollisionCircleRec(ball.position, ball.radius, (Rectangle){p1.x,p1.y,p1.w,p1.h})) { // Ball hit left paddle?
            ball.velocity.x = -ball.velocity.x*1.05f; // Reverse X and speed up 5%
            ball.position.x = p1.x+p1.w+ball.radius+1; // Push ball out to avoid sticking
            if (difficulty==4) { // Variable mode: randomize speed on paddle hit
                float v = 0.5f + (float)GetRandomValue(0,150)/100.0f; // 0.5-2.0
                float ang = atan2f(ball.velocity.y, ball.velocity.x); // Current angle
                float ns = 400 * v; // New magnitude
                ball.velocity.x = cosf(ang)*ns; ball.velocity.y = sinf(ang)*ns; // Apply
                ballSF = v; float ps2 = 600 * (0.8f + 0.4f*ballSF); // Store and rescale paddles
                p1.speed = ps2; p2.speed = ps2;
                varTimer = 0.0f; // Reset slow timer
            }
        }
        if (CheckCollisionCircleRec(ball.position, ball.radius, (Rectangle){p2.x,p2.y,p2.w,p2.h})) { // Ball hit right paddle?
            ball.velocity.x = -ball.velocity.x*1.05f; // Reverse X, +5%
            ball.position.x = p2.x-ball.radius-1; // Push out leftwards
            if (difficulty==4) { // Same randomization for right paddle
                float v = 0.5f + (float)GetRandomValue(0,150)/100.0f;
                float ang = atan2f(ball.velocity.y, ball.velocity.x);
                float ns = 400 * v;
                ball.velocity.x = cosf(ang)*ns; ball.velocity.y = sinf(ang)*ns;
                ballSF = v; float ps2 = 600 * (0.8f + 0.4f*ballSF);
                p1.speed = ps2; p2.speed = ps2;
                varTimer = 0.0f;
            }
        }

        if (s1>=target || s2>=target) { // Someone reached target points? Game over
            if (s1>=target) g1++; else g2++; // Increment games won
            s1=0; s2=0; // Reset points for next game
            ResetBall(&ball,W,H, difficulty==4? 1.0f : ballSF); // Reset ball, variable mode resets to 1.0x
            if (difficulty==4) ballSF=1.0f; // Reset factor display
            float psr = 600 * (0.8f + 0.4f*ballSF); // Recalc paddle speed
            p1.speed = psr; p2.speed = psr; // Apply
            varTimer = 0.0f; // Reset timer
            int need = bestOf/2 + 1; // Games needed to win series, e.g. best of 3 -> 2
            if (g1>=need || g2>=need) seriesOver=true; // Check series win
            else { // Otherwise start next game
                p1.y = H/2 - p1.h/2; // Center paddles vertically
                p2.y = H/2 - p2.h/2;
                countdown=5.0f; // Restart countdown
                BeginDrawing(); ClearBackground(BLACK); // Show brief message
                DrawText(TextFormat("Game over! Series %d - %d", g1, g2), W/2-300, H/2-30, 40, GREEN);
                EndDrawing(); WaitTime(1.2); // Pause 1.2 sec so message is visible
            }
        }

        BeginDrawing(); // Start gameplay render
        ClearBackground(BLACK); // Clear
        PaddleDraw(p1); PaddleDraw(p2); BallDraw(ball); // Draw paddles and ball
        ScoreDraw(s1,s2,n1,n2,W,target); // Draw scores/names/target
        DrawText(TextFormat("Games: %d - %d (Best of %d)", g1, g2, bestOf), W/2-160, 95, 26, GRAY); // Series status
        if (difficulty==4) DrawText(TextFormat("VAR: %.2fx", ballSF), 60, 95, 28, YELLOW); // Show variable speed in yellow
        else DrawText(TextFormat("%.2fx", ballSF), 60, 95, 28, GRAY); // Show fixed speed in gray
        EndDrawing(); // Present frame
    }
    CloseWindow(); // Close raylib window, cleanup
    return 0; // Exit success
}
