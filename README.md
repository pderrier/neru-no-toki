# ZeN / WondY DOS Demos — SDL2 Port

Demoscene effects originally written for DOS in 1996–1999 (Turbo C++, Watcom C, DJGPP, Turbo Pascal), ported to modern C99 + SDL2.

These demos were created by **WondY** and **ZeN** and originally ran in VGA Mode 13h (320×200), Mode X (320×240), or via the PTC truecolor library on top of DOS4GW. They used MIDAS Sound System and USMPlay for tracker music (.XM/.MOD playback).

This port preserves the original algorithms — the inline x86 assembly has been rewritten in plain C, all hardware-specific code (VGA registers, BIOS interrupts, DOS extenders) has been replaced by SDL2, and the original tracker music now plays via SDL2_mixer.

## Build

**Requires:** CMake ≥ 3.16, a C99 compiler, SDL2 + SDL2_mixer development libraries.

### Windows (vcpkg)

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg install sdl2:x64-windows sdl2-mixer:x64-windows

cd <this-repo>
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release
```

### Windows (MSYS2 / MinGW)

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-cmake
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

### Linux / WSL

```bash
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-mixer-dev
cmake -B build
cmake --build build
```

### macOS

```bash
brew install sdl2 sdl2_mixer cmake
cmake -B build
cmake --build build
```

## Run

Each demo is a standalone executable. **Run from the repo root** (or make sure the `data/` folder is next to the executable) so the music files are found.

```bash
# Linux / WSL / macOS
./build/01_flames

# Windows
build\Release\01_flames.exe
```

Press **Escape** to quit any demo.

## Demos

### Neru No Toki — "Le moment ou l'on se repose" (ZeN, 1999)

The full **Neru No Toki** demo is included as `16_nnt`. It chains 6 parts synchronized to two XM tracks (EXEL.XM, ALMA_DEL.XM): photo overlays with directional blur, a rotozoom with poem text, an animated yoga scene reflected in water, a parallax fog landscape with scrolling credits, and a raycast engine morphing between a deformed plane, metaspheres and a tunnel. All original 24-bit PCX assets are used.

```bash
./build/16_nnt
```

Credits:
- **Wondy** / ZeN — FX2D rotozoom, KATA yoga scene, CHAOS landscape, PCX loaders
- **Prozero** / ZeN — Raycast engine, CHAOS co-code, SALETE overlay
- **Decibel** / ZeN — Radial blur
- **Aerial** / ZeN — Music (EXEL.XM)
- **Lluvia** / Ribbon — Music (ALMA_DEL.XM, "Alma del Vento")

Almost presented at [Volcanic 5](https://demozoo.org/parties/739/) (Cournon-d'Auvergne, France, February 1999). Originally built with Watcom C++ 11.0, DOS4GW, PTC 0.60 and MIDAS Sound System. The [original source code](original/) is included with a write-up on the x86 assembly tricks, the all-night coding sessions, and what it meant to code demos on a 486 in 1999.

### Individual effects and other stuff

| # | Name | Effect | Music | Original | Controls |
|---|------|--------|-------|----------|----------|
| 01 | `flames` | Fire with thermal diffusion | KAOSSONG.MOD | Wondy 08/96, Pascal | — |
| 02 | `fraktal` | Mandelbrot + palette rotation | KAOSSONG.MOD | Wondy 12/96 | — |
| 03 | `chaos` | Logistic map bifurcation | songsect.xm | WondY / ZeN | `+`/`-` iterations |
| 04 | `starfly` | 3D starfield | songsect.xm | Wondy 10/96 | Arrows / WASD, `+`/`-` speed |
| 05 | `shadebob` | Thermal blob following mouse | KAOSSONG.MOD | Wondy 07/96 | Mouse |
| 06 | `tunnel` | Precalculated textured tunnel | DISCO.XM | Wondy 11/96 | — |
| 07 | `pixelreb` | Bouncing particles on terrain | songsect.xm | Wondy 96 | — |
| 08 | `wobbler` | Sine-table image distortion | templsun.xm | Wondy 96 | — |
| 09 | `sinewave` | Horizontal sine bars | DISCO.XM | Wondy 96 | `Space` toggle mode |
| 10 | `plasma` | Interactive heat diffusion | templsun.xm | Wondy 96 | Click to draw |
| 11 | `voxel` | Voxel terrain engine | ALMA_DEL.XM | WondY / ZeN, Watcom | Arrows, `Q`/`W` altitude |
| 12 | `particles` | Additive-blended particle system | DISCO.XM | WondY / ZeN, DJGPP | — |
| 13 | `rotozoom` | Rotozoom + tunnel + motion blur | DISCO.XM | WondY / ZeN 98, PTC | — |
| 14 | `w3d` | Software 3D engine (torus) | EXEL.XM | WondY / ZeN 98 | `M` blur, `R` rotate, `Z`/`S` zoom, Mouse |
| 15 | `rat` | Autonomous maze agent | ZINZIN.XM | WondY / ZeN, Watcom | — |
| **16** | **`nnt`** | **Full demo: Neru No Toki** | **EXEL.XM → ALMA_DEL.XM** | **ZeN 1999** | — |

## Project structure

```
include/
  demo_framework.h    SDL2/SDL2_mixer framework (replaces VGA/PTC/Mode 13h/MIDAS)
src/
  01_flames.c         One self-contained file per demo
  02_fraktal.c
  ...
  15_rat.c
  16_nnt.c            Full NNT demo (all parts sequenced with original assets)
data/
  ALMA_DEL.XM         Original tracker music from the demos
  DISCO.XM
  EXEL.XM
  KAOSSONG.MOD
  ZINZIN.XM
  songsect.xm
  templsun.xm
  nnt/                Original NNT demo assets (PCX textures, XM music)
CMakeLists.txt
LICENSE
```

## What was ported and how

| DOS original | SDL2 replacement |
|---|---|
| VGA Mode 13h (`int 10h`, `0xA0000`) | SDL2 window + ARGB8888 texture |
| Mode X (unchained VGA, page flipping) | Same SDL2 framebuffer |
| PTC 0.60 truecolor library | Same SDL2 framebuffer |
| DOS4GW 32-bit extender | Not needed (native 64-bit) |
| VBL sync (`in al,0x3DA`) | SDL2 VSync (`SDL_RENDERER_PRESENTVSYNC`) |
| VGA palette DAC (`out 0x3C8`) | Software palette → ARGB conversion |
| Inline x86 assembly | Rewritten in plain C99 |
| MIDAS Sound System (.XM) | SDL2_mixer `Mix_LoadMUS` / `Mix_PlayMusic` |
| USMPlay (.XM) | SDL2_mixer |
| `kbhit()` / `getch()` / `inp(0x60)` | `SDL_PollEvent` / `SDL_GetKeyboardState` |
| `int 33h` mouse driver | `SDL_GetMouseState` |
| Turbo C `lib13h.h` / `modex.h` | `demo_framework.h` |
| PCX image loading | `load_pcx()` in framework |

## Credits

- **WondY** / **ZeN** — original DOS code and music (1996–1999)
- SDL2 port — 2026

## License

[MIT](LICENSE)
