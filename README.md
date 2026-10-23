# Games — Eigencube v9 - 1820 Edition + Pong

Monorepo of Guix / raylib games by Don.

## Eigencube v9 - 1820 Edition [CURRENT]

**No default player — starts clean.**

- `Player: "" ?` on launch — no "Don" default
- `+ PLAYER` required to start — type name + ENTER
- Dropdown shows `scores/*.dat` players — click to switch
- `PB: ---` until first score, then shows Personal Best
- `TOP 5` starts empty `---` — fills on `END GAME` or solve
- `READY TO PLAY` scrambles 50 moves
- `END GAME` saves score + personal best + cleans empty files
- `EXIT` **zeroes all** — `memset(top5,0)`, `rm -rf scores/*.dat scores.dat`, `playerName=""` → next launch is clean again
- Unified drag: L-drag tile Row/Col (top/bottom/sides fixed) | R-drag Orbit | Wheel Zoom
- 1820x1024 window, 2-col TOP5, Guile scheme live reload `scheme/live.scm`

### Build & Run
```bash
cd eigencube
guix shell guile@3.0 raylib glfw pkg-config gcc-toolchain -- make clean all
./eigencube
