# Games — opendon53now

[![Guix](https://img.shields.io/badge/Guix-2026-blue?logo=gnu)](https://guix.gnu.org)
[![raylib](https://img.shields.io/badge/raylib-5.5-black)](https://www.raylib.com)
[![GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-green)](LICENSE)

Persistent collection — outside /gnu/store, ~/.emacs.d, flatpak.

## Games
- **eigencube/** - 54 Tile 3D Cube v8.5.1 - timer starts on first move
- **pong/** - Classic Pong C + raylib

Build: `guix shell raylib guile -- make -C eigencube && make -C pong`
Run: `./eigencube/eigencube` and `./pong/pong`
