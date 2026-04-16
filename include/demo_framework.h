// demo_framework.h - SDL2 framework replacing DOS VGA/PTC/Mode13h
// Ported from ZeN/WondY DOS demos (1996-1999)
#ifndef DEMO_FRAMEWORK_H
#define DEMO_FRAMEWORK_H

#include <SDL.h>
#if __has_include(<SDL_mixer.h>)
#include <SDL_mixer.h>
#define HAS_SDL_MIXER 1
#else
#define HAS_SDL_MIXER 0
// Stubs so code compiles without SDL2_mixer
typedef void Mix_Music;
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Screen dimensions (original DOS modes)
#define SCREEN_W 320
#define SCREEN_H 200
#define SCREEN_W_240 320
#define SCREEN_H_240 240
#define SCALE 3 // Window scale factor

// Color helpers
#define RGB32(r,g,b) ((0xFF000000)|((r)<<16)|((g)<<8)|(b))
#define RGB32_FROM_PAL(pal,idx) RGB32((pal)[(idx)*3]*4, (pal)[(idx)*3+1]*4, (pal)[(idx)*3+2]*4)

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 *pixels;       // ARGB8888 framebuffer
    int width;
    int height;
    int running;
    Uint32 frame_start;
} DemoContext;

static inline DemoContext* demo_init(const char *title, int w, int h) {
    DemoContext *ctx = (DemoContext*)calloc(1, sizeof(DemoContext));
    if (!ctx) return NULL;

    ctx->width = w;
    ctx->height = h;
    ctx->running = 1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        free(ctx);
        return NULL;
    }

    ctx->window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w * SCALE, h * SCALE, SDL_WINDOW_SHOWN);
    if (!ctx->window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(ctx);
        return NULL;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ctx->renderer) {
        ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_SOFTWARE);
    }

    SDL_RenderSetLogicalSize(ctx->renderer, w, h);

    ctx->texture = SDL_CreateTexture(ctx->renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);

    ctx->pixels = (Uint32*)calloc(w * h, sizeof(Uint32));

#if HAS_SDL_MIXER
    // Init audio mixer (44100Hz, default format, 2 channels, 2048 sample buffer)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s (continuing without sound)\n", Mix_GetError());
    }
#endif

    return ctx;
}

static inline void demo_update(DemoContext *ctx) {
    SDL_UpdateTexture(ctx->texture, NULL, ctx->pixels, ctx->width * sizeof(Uint32));
    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
    SDL_RenderPresent(ctx->renderer);
}

static inline int demo_poll(DemoContext *ctx) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) ctx->running = 0;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) ctx->running = 0;
    }
    return ctx->running;
}

// Combined poll + update + frame timing (target ~60fps via vsync)
static inline int demo_frame(DemoContext *ctx) {
    demo_update(ctx);
    return demo_poll(ctx);
}

static inline void demo_close(DemoContext *ctx) {
    if (!ctx) return;
#if HAS_SDL_MIXER
    Mix_CloseAudio();
#endif
    free(ctx->pixels);
    SDL_DestroyTexture(ctx->texture);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    SDL_Quit();
    free(ctx);
}

// ---- Music (tracker module) playback ----

static inline Mix_Music* demo_load_music(const char *filename) {
#if HAS_SDL_MIXER
    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) fprintf(stderr, "Cannot load music %s: %s\n", filename, Mix_GetError());
    return music;
#else
    (void)filename;
    fprintf(stderr, "No SDL2_mixer: music disabled (%s)\n", filename);
    return NULL;
#endif
}

static inline void demo_play_music(Mix_Music *music) {
#if HAS_SDL_MIXER
    if (music) Mix_PlayMusic(music, -1); // loop forever
#else
    (void)music;
#endif
}

static inline void demo_play_music_once(Mix_Music *music) {
#if HAS_SDL_MIXER
    if (music) Mix_PlayMusic(music, 0); // play once, no loop
#else
    (void)music;
#endif
}

static inline int demo_music_playing(void) {
#if HAS_SDL_MIXER
    return Mix_PlayingMusic();
#else
    return 0;
#endif
}

static inline void demo_stop_music(void) {
#if HAS_SDL_MIXER
    Mix_HaltMusic();
#endif
}

static inline void demo_free_music(Mix_Music *music) {
#if HAS_SDL_MIXER
    if (music) Mix_FreeMusic(music);
#else
    (void)music;
#endif
}

static inline void demo_set_volume(int vol) {
#if HAS_SDL_MIXER
    Mix_VolumeMusic(vol);
#else
    (void)vol;
#endif
}

// Utility: put pixel with bounds checking
static inline void put_pixel(DemoContext *ctx, int x, int y, Uint32 color) {
    if (x >= 0 && x < ctx->width && y >= 0 && y < ctx->height)
        ctx->pixels[y * ctx->width + x] = color;
}

// Utility: put pixel using 8-bit palette index
static inline void put_pixel_pal(DemoContext *ctx, int x, int y, unsigned char idx, const unsigned char *pal) {
    if (x >= 0 && x < ctx->width && y >= 0 && y < ctx->height)
        ctx->pixels[y * ctx->width + x] = RGB32(pal[idx*3]*4, pal[idx*3+1]*4, pal[idx*3+2]*4);
}

// Utility: horizontal line
static inline void hline(DemoContext *ctx, int x, int y, int len, Uint32 color) {
    if (y < 0 || y >= ctx->height) return;
    if (x < 0) { len += x; x = 0; }
    if (x + len > ctx->width) len = ctx->width - x;
    for (int i = 0; i < len; i++)
        ctx->pixels[y * ctx->width + x + i] = color;
}

// Utility: simple PCX 256-color loader (8-bit indexed)
// Returns pixel data, fills palette[768] (VGA 6-bit values)
static inline unsigned char* load_pcx(const char *filename, unsigned char *palette, int *out_w, int *out_h) {
    FILE *f = fopen(filename, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", filename); return NULL; }

    unsigned char header[128];
    if (fread(header, 1, 128, f) != 128) { fclose(f); return NULL; }

    int xmin = header[4] | (header[5] << 8);
    int ymin = header[6] | (header[7] << 8);
    int xmax = header[8] | (header[9] << 8);
    int ymax = header[10] | (header[11] << 8);
    int w = xmax - xmin + 1;
    int h = ymax - ymin + 1;

    unsigned char *pixels = (unsigned char*)malloc(w * h);
    if (!pixels) { fclose(f); return NULL; }

    int x = 0, y = 0;
    while (y < h) {
        int c = fgetc(f);
        if (c == EOF) break;
        int count = 1;
        if ((c & 0xC0) == 0xC0) {
            count = c & 0x3F;
            c = fgetc(f);
            if (c == EOF) break;
        }
        for (int i = 0; i < count && y < h; i++) {
            pixels[y * w + x] = (unsigned char)c;
            x++;
            if (x >= w) { x = 0; y++; }
        }
    }

    // Read palette (at end of file, after 0x0C marker)
    if (palette) {
        int marker = fgetc(f);
        if (marker == 0x0C) {
            fread(palette, 1, 768, f);
            // PCX palette is 8-bit, convert to VGA 6-bit
            for (int i = 0; i < 768; i++) palette[i] /= 4;
        }
    }

    fclose(f);
    if (out_w) *out_w = w;
    if (out_h) *out_h = h;
    return pixels;
}

// Get mouse position relative to logical resolution
static inline void demo_get_mouse(DemoContext *ctx, int *mx, int *my) {
    int wx, wy;
    SDL_GetMouseState(&wx, &wy);
    // Account for logical size scaling
    float sx, sy;
    SDL_RenderGetScale(ctx->renderer, &sx, &sy);
    *mx = (int)(wx / SCALE);
    *my = (int)(wy / SCALE);
    if (*mx >= ctx->width) *mx = ctx->width - 1;
    if (*my >= ctx->height) *my = ctx->height - 1;
    if (*mx < 0) *mx = 0;
    if (*my < 0) *my = 0;
}

static inline int demo_mouse_pressed(void) {
    return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK) != 0;
}

#endif // DEMO_FRAMEWORK_H
