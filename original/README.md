# Neru No Toki - Original DOS Source Code

**"Le moment ou l'on se repose"** - ZeN, 1999

This is the original source code of the Neru No Toki demo, written for DOS in 1998-1999. It was compiled with Watcom C++ 11.0 targeting DOS4GW (32-bit protected mode DOS extender) and linked against PTC 0.60 (truecolor graphics) and MIDAS Sound System (XM tracker playback).

This code will not compile on any modern system. It is preserved here for historical and educational value.

## Technical context: what it meant to code demos in 1998-1999

**No GPU.** Everything is software-rendered into a RAM buffer, then blitted to the screen through PTC (which handled VESA mode setting and bank switching). Every pixel is computed by the CPU.

**No floating point unit (sometimes).** The 486SX had no FPU. The 486DX and Pentium did, but FP was still slow. That's why there are so many integer-only inner loops and lookup tables. The raycast precalculates directions as floats during init, but the per-pixel interpolation in `INTERPOL.H` is all 16.16 fixed-point integer math.

**640KB base memory, 16-32MB extended.** DOS4GW gave access to extended memory via DPMI (DOS Protected Mode Interface), turning the 386+ into a flat 32-bit address space. Without it, you were stuck in 640KB real mode.

**No multithreading.** One CPU, one thread. The music (MIDAS) ran in a timer interrupt, stealing cycles from the main loop. `inp(0x60)` reads the keyboard port directly - there's no OS keyboard queue.

**No standard display interface.** VGA Mode 13h gave you 320x200 at 8-bit color by writing to address `0xA0000`. For truecolor (320x240 at 32bpp), you needed PTC or your own VESA driver, plus UniVBE to patch broken BIOS implementations. Every video card behaved differently.

**The compile-test cycle was: edit in a DOS text editor, run MAK.BAT, watch the demo, Ctrl+Alt+Del if it crashes, repeat.** No debugger. `printf` before the crash was your only friend. The demo either worked or it hung your machine.

## The MAKING.OF

Two notes written during an all-night coding session:

> **Prozero (21:06):** "midas marche pas top sur ton pc / j'ai linke gs (rien de dur) / je l'ai rajoute sur nntfx1 mais y'a des bugs (evidents a comprendre et corriger) / je dors..."

> **Wondy (5:01):** "Bon moi je rends la main, la / A tchao les frangins, reveillez moi pour les compos / za+"

They were taking turns coding through the night, passing the keyboard.

### The build script talks to you

`MAK.BAT` prints ASCII box art for each stage. Compilation errors trigger "META-PROUT DE COMPILATION". Success prints "Et zou !" and immediately launches the demo via UniVBE (a VESA BIOS extension driver that was required for SVGA on most cards).

### ZIK.BAT is peak 1998

```batch
cd e:\zik\ft2
ft2 \encours\nnt\final\data\alma_del.xm
call h.bat
```

The music was edited live in FastTracker II from a hardcoded path on Wondy's hard drive. `h.bat` is lost to time.

## What is actually nice and well-optimized

### Inline x86 assembly where it matters

The FX2D rotozoom inner loop (`NNT.H:Do_FX2D`) has a comment "il reste des AGIs..." - referring to **Address Generation Interlocks**, a Pentium-era pipeline stall that occurs when you use a register as an address immediately after modifying it. The coder was aware of CPU pipeline behavior and tried to interleave instructions to avoid stalls:

```asm
mov eax,y
mov esi,x
mov ebx,eax        ; use eax before modifying
add eax,dyang1
add ebx,dyang2     ; parallel work on different regs
mov edi,esi
shl eax,7          ; address calc for eax
add esi,dxang1
shl ebx,7          ; address calc for ebx interleaved
```

The `SALETE_Go` function in `NNT.H` uses carefully structured assembly to darken random screen columns and overlay dirt sprites, with manual carry-flag checking (`jnc`) for saturated subtraction - there was no `PSUBUSB` without MMX.

### The radial blur is clever

`RADIAL_Go` (`NNT.H`) precalculates massive sine/cosine tables (640x512 entries for X and 480x512 for Y) during init, then the inner loop is just table lookups and averaging. Trading ~10MB of RAM for zero trig calls per pixel was a bold choice on machines with 16-32MB total.

### The raycast engine is genuinely impressive

`RAYCAST.H` implements three different raycast geometries (deformed heightmap plane, organic tunnel with variable radius, metaspheres) and smoothly morphs between them using linear interpolation of the UV/lighting parameters. It renders at 40x25 resolution and bilinearly interpolates to 320x200, which was a common trick to get complex per-pixel math running at interactive framerates on a 486/Pentium.

The tunnel equation uses the actual quadratic formula for ray-cylinder intersection:
```c
float a = d.x*d.x + d.y*d.y;
float rayon = (sin(5*atan2(d.x,d.y))*tuqtenoise) + 256;
float c = o.x*o.x + o.y*o.y - rayon*rayon;
float delta = b*b - 4*a*c;
t = (-b+sqrt(delta)) / (2*a);
```

The `5*atan2` in the radius creates the organic five-lobed tunnel deformation. Real math, done per low-res block, interpolated to full screen.

### The CHAOS landscape parallax

Three layers scrolling at different speeds (fond at +4/frame, bump at +6, posx3 at +8), composited with bump-mapped fog and directional-blurred text overlay. The blur kernel size oscillates via a sine table (`_sin[compteur]`), creating the pulsing text focus effect. Simple techniques, combined well.

### PCX 24-bit planar loading

`COMMON.H` contains a proper 24-bit PCX decoder for planar RGB images (red plane, then green plane, then blue plane - the PCX format stores them separately). Three parallel decode loops with RLE decompression. This was the image format of choice because Deluxe Paint and other DOS paint programs exported it natively.

## What is clearly badly done (and that's fine)

**Everything is in headers.** All the effect code lives in `.H` files, included directly into `NNT.CPP`. There are no separate compilation units for effects. The entire demo is essentially one giant translation unit. This was common in the demoscene - ship it, don't architect it.

**Global variables everywhere.** `NNTVAR.H` is 142 lines of raw globals: `int x,y,i,j,ofs;` shared across all effects. Variable names like `pouet`, `ploup`, `zob` speak for themselves.

**`goto` as flow control.** The PCX loaders use `goto BOUCLE_CHARGE` for their decode loops. The original tunnel effect uses `goto` for its main loop. This was normal in 1996 C.

**No error handling.** Most `malloc` calls check for NULL but then just call `{TERMINE}` which is a macro that prints "Abnormal Termination, mon frere..." and exits. No cleanup, no resource freeing. If it fails, you reboot.

**Hardcoded paths.** `ZIK.BAT` points to `e:\zik\ft2`. The compile script assumes `e:\libs\ptc060`. Everyone had one PC and one hard drive layout.

## Files

| File | What it is |
|------|-----------|
| `NNT.CPP` | Main file - loads all assets, inits all effects, runs the demo sequence |
| `MAKING.OF` | Notes scribbled at night between Prozero and Wondy during the coding session |
| `MAK.BAT` | Build script - "Femmes enceintes s'abstenir", "META-PROUT DE COMPILATION" |
| `MAKE.LNK` | Watcom linker config: nnt.obj + proc.obj against midas.lib + ptc.lib, `dos4g`, 256K stack |
| `ZIK.BAT` | Launches FastTracker II to edit the music. Two lines of batch, a whole vibe |
| `include/COMMON.H` | Types, PCX loaders (8-bit and 24-bit), VBL sync routine |
| `include/NNT.H` | All the visual effects: radial blur, FX2D rotozoom, KATA yoga scene, CHAOS landscape, SALETE overlay |
| `include/NNTVAR.H` | Every global variable in the demo, all in one file |
| `include/INTERPOL.H` | Bilinear interpolation for the raycast renderer |
| `include/RAYCAST.H` | Raycast engine: deformed plane, tunnel, metaspheres, with smooth transitions |
