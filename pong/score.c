#include "score.h"
#include "raylib.h"
#include <math.h>
void ScoreDraw(int s1, int s2, const char *n1, const char *n2, int W, int target) {
	    
    float hue = fmodf(GetTime()*40.0f, 360.0f);
    Color spec = ColorFromHSV(hue, 1.0f, 1.0f);

    DrawText(TextFormat("%s %d : %d %s", n1, s1, s2, n2), W/2 - 140, 20, 30, spec);
    DrawText(TextFormat("First to %d", target), W/2 - 60, 55, 20, GRAY);
}
