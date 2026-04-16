# ZeN / WondY DOS Demos — SDL2 Port

Demoscene effects originally written for DOS in 1996–1999 (Turbo C++, Watcom C, DJGPP, Turbo Pascal), ported to modern C99 + SDL2.

These demos were created by **WondY** and **ZeN** and originally ran in VGA Mode 13h (320×200), Mode X (320×240), or via the PTC truecolor library on top of DOS4GW. They used MIDAS Sound System and USMPlay for tracker music (.XM/.MOD playback).

This port preserves the original algorithms — the inline x86 assembly has been rewritten in plain C, all hardware-specific code (VGA registers, BIOS interrupts, DOS extenders) has been replaced by SDL2, and the original tracker music now plays via SDL2_mixer.

## Technical context

### No GPU, only pain

Late-90s DOS demo coding meant one thing: the CPU did everything. No shaders, no OpenGL, no safety net. Every pixel was computed in software, usually into a framebuffer in RAM, then blasted to VGA memory as fast as possible before the next frame showed up.

That is the world behind `01_flames`, `02_fraktal`, `05_shadebob`, `09_sinewave`, and `10_plasma`: not "effects plugins", just raw pixel pushing with a lot of attitude.

### Fast Or Dead

If something was expensive, you cheated. Trig became lookup tables. Floats became fixed-point. Full-screen rendering became low-res rendering plus interpolation. Recomputing became incrementing.

That is why `06_tunnel` uses precalculated addressing, `11_voxel` is basically a smart column renderer, `13_rotozoom` walks texture coordinates incrementally, and `16_nnt` squeezes complex raycast scenes out of low-res buffers and reconstruction tricks.

### RAM Was A Feature

Memory was tight, but CPU time was tighter. So coders happily burned kilobytes, sometimes megabytes, on sine tables, palette ramps, blur maps, distortion fields, heightmaps, and anything else that could save cycles in the inner loop.

You can see that mindset all over `08_wobbler`, `11_voxel`, `12_particles`, `14_w3d`, and the larger `16_nnt` parts: precompute first, stream linearly, pray for cache, ship the effect.

### ASM In The Basement

Hot loops were often handwritten in x86 assembly because compilers were not trusted to do the right thing when every cycle mattered. Register usage, memory access, and instruction ordering were part of the art form.

So when these effects look unexpectedly smooth on period hardware, that was usually not magic. It was tables, integer math, tiny buffers, dirty tricks, and a coder staring at the same inner loop until it finally stopped being too slow.

### Music Was Code Too

The `.XM` and `.MOD` files in `data/` are tracker modules: samples plus note/effect patterns, not recorded audio. They were compact, flexible, and perfect for demos. A playback library such as MIDAS or USMPlay mixed them in software while the main code was still busy drawing pixels.

That is why the music is not just background. In `16_nnt` especially, it is part of the structure. Scene timing, mood changes, and transitions are built around the module timeline, just like in the original productions.

### Why It Looks Like This

Flames, plasmas, tunnels, voxels, particles, rotozooms, and software 3D were not just aesthetic choices. They were the shapes that looked great when your real tools were a 486, a framebuffer, a tracker module, and a slightly unreasonable amount of persistence.

### Museum Highlights

Some period-correct techniques and habits you will spot here that feel almost alien today:

- `Framebuffer First`: draw into memory first, then slam the result to the screen. No scene graph, no GPU pipeline, no abstraction tower.
- `Palette Wizardry`: in 8-bit modes, changing colors in the palette could animate the whole screen "for free". Great for flames, fades, and fake lighting.
- `Table All The Things`: sine, cosine, blur offsets, tunnel maps, distortion fields. If a value could be precomputed once, it probably was.
- `Fixed-Point Supremacy`: integer adds and shifts were often a better life choice than floating point.
- `Low-Res, High Impact`: many expensive effects were rendered small, then stretched, blurred, or interpolated to full screen.
- `One Big Ball Of Globals`: one file full of globals, one giant translation unit, and very little concern for architectural elegance.
- `ASM In Case Of Emergency`: not everywhere, just in the loops that were actively ruining the framerate.
- `Music vs CPU`: the soundtrack kept mixing while the main loop was busy drawing, which means visuals and audio were literally competing for CPU time.
- `Hardcoded Reality`: specific compilers, specific sound libs, specific machine setup, sometimes specific drive letters. Portability was not invited.
- `Reboot As Debugger`: run it, see if it works, and if it hangs the machine, that was also valid feedback.

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

## Port and fidelity

The goal of the port is effect fidelity, not hardware-exact emulation. For the shared effects, this is the practical translation from late-90s DOS code to modern SDL2:

| Original DOS world | Modern port | What stayed faithful |
|---|---|---|
| VGA Mode 13h / Mode X / PTC framebuffer code | SDL2 software framebuffer + texture upload | The effect logic, layering, and overall screen look |
| Hardware palette writes, DAC tricks, page flipping | Software palette conversion and modern presentation | Palette-driven fades, flames, fake lighting, and indexed-color behavior |
| VBL polling and CPU-speed-dependent pacing | SDL timing and VSync where available | Scene flow and interaction, without pretending to match old hardware timing cycle-for-cycle |
| Inline x86 assembly inner loops | Portable C99 rewrites | The same algorithms and dataflow, but not the original instruction stream |
| DOS4GW, BIOS calls, direct hardware access | Native host process + SDL2 abstractions | The same software-rendered mindset, without DOS-specific plumbing |
| MIDAS / USMPlay playback under DOS | SDL2_mixer music playback | The original tracker modules and their role in timing and atmosphere |
| Shared effect code tied to one DOS codebase and lots of globals | Shared portable helpers and per-demo integration | The same bag of tricks: lookup tables, low-res shortcuts, fixed-point style thinking, and software rendering |
| DOS input paths such as `kbhit()`, `getch()`, `inp(0x60)`, `int 33h` | SDL keyboard and mouse events | The interactive behavior of the original effects |
| Original PCX loading and asset formats | Portable loaders in the framework | The original textures, images, and demo assets |

So the target is "same effect, same spirit, same tricks", not a cycle-perfect clone of a 1999 DOS PC.

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

## Run

Each demo is a standalone executable. **Run from the repo root** (or make sure the `data/` folder is next to the executable) so the music files are found.

```bash
# Linux / WSL / macOS
./build/01_flames

# Windows
build\Release\01_flames.exe
```

Press **Escape** to quit any demo.

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

## Credits

- **WondY** / **ZeN** — original DOS code and music (1996–1999)
- SDL2 port — 2026

## License

[MIT](LICENSE)
