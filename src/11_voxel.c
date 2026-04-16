// VOXEL - Original by WondY / ZeN (Watcom)
// Voxel terrain engine with height/color maps
// Ported from Watcom C Mode 13h to SDL2

#include "demo_framework.h"

#define MAP_SIZE 256
#define VW 320
#define VH 200

unsigned char HMap[MAP_SIZE * MAP_SIZE]; // Height map
unsigned char CMap[MAP_SIZE * MAP_SIZE]; // Color map
unsigned char palette[768];

int lasty[VW];
int lastc[VW];

int clamp(int x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }

void compute_map(void) {
    // Generate fractal height map
    HMap[0] = 128;
    for (int p = MAP_SIZE; p > 1; p = p >> 1) {
        int p2 = p >> 1;
        int k = p * 8 + 20;
        int k2 = k >> 2;
        for (int i = 0; i < MAP_SIZE; i += p) {
            for (int j = 0; j < MAP_SIZE; j += p) {
                int a = HMap[(i << 8) + j];
                int b = HMap[(((i + p) & 255) << 8) + j];
                int c = HMap[(i << 8) + ((j + p) & 255)];
                int d = HMap[(((i + p) & 255) << 8) + ((j + p) & 255)];

                HMap[(i << 8) + ((j + p2) & 255)] = clamp(((a + c) >> 1) + (rand() % k - k2));
                HMap[(((i + p2) & 255) << 8) + ((j + p2) & 255)] = clamp(((a + b + c + d) >> 2) + (rand() % k - k2));
                HMap[(((i + p2) & 255) << 8) + j] = clamp(((a + b) >> 1) + (rand() % k - k2));
            }
        }
    }

    // Smooth
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < MAP_SIZE * MAP_SIZE; i += MAP_SIZE)
            for (int j = 0; j < MAP_SIZE; j++)
                HMap[i + j] = (HMap[((i + MAP_SIZE) & 0xFF00) + j] +
                               HMap[i + ((j + 1) & 0xFF)] +
                               HMap[((i - MAP_SIZE) & 0xFF00) + j] +
                               HMap[i + ((j - 1) & 0xFF)]) >> 2;

    // Color = derivative of height
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i += MAP_SIZE)
        for (int j = 0; j < MAP_SIZE; j++) {
            int k = 128 + (HMap[((i + MAP_SIZE) & 0xFF00) + ((j + 1) & 255)] - HMap[i + j]) * 4;
            if (k < 0) k = 0; if (k > 255) k = 255;
            CMap[i + j] = k;
        }

    // Generate terrain palette
    for (int i = 0; i < 64; i++) {
        palette[i * 3] = (unsigned char)(i / 4);        // dark water
        palette[i * 3 + 1] = (unsigned char)(i / 2);
        palette[i * 3 + 2] = (unsigned char)(i);
    }
    for (int i = 64; i < 128; i++) {
        palette[i * 3] = (unsigned char)((i - 64) / 2); // green
        palette[i * 3 + 1] = (unsigned char)(16 + (i - 64));
        palette[i * 3 + 2] = (unsigned char)((i - 64) / 4);
    }
    for (int i = 128; i < 192; i++) {
        palette[i * 3] = (unsigned char)(32 + (i - 128) / 2); // brown
        palette[i * 3 + 1] = (unsigned char)(32 + (i - 128) / 2);
        palette[i * 3 + 2] = (unsigned char)(16 + (i - 128) / 4);
    }
    for (int i = 192; i < 256; i++) {
        palette[i * 3] = (unsigned char)(48 + (i - 192) / 4);  // white peaks
        palette[i * 3 + 1] = (unsigned char)(48 + (i - 192) / 4);
        palette[i * 3 + 2] = (unsigned char)(48 + (i - 192) / 4);
    }
}

void draw_line(Uint32 *pixels, int x0, int y0, int x1, int y1, int hy, int s) {
    int sx = (x1 - x0) / VW;
    int sy = (y1 - y0) / VW;

    for (int i = 0; i < VW; i++) {
        int u0 = (x0 >> 16) & 0xFF;
        int a = (x0 >> 8) & 255;
        int v0 = (y0 >> 8) & 0xFF00;
        int b = (y0 >> 8) & 255;
        int u1 = (u0 + 1) & 0xFF;
        int v1 = (v0 + 256) & 0xFF00;

        int h0 = HMap[u0 + v0], h2 = HMap[u0 + v1];
        int h1 = HMap[u1 + v0], h3 = HMap[u1 + v1];

        h0 = (h0 << 8) + a * (h1 - h0);
        h2 = (h2 << 8) + a * (h3 - h2);
        int h = ((h0 << 8) + b * (h2 - h0)) >> 16;

        h0 = CMap[u0 + v0]; h2 = CMap[u0 + v1];
        h1 = CMap[u1 + v0]; h3 = CMap[u1 + v1];
        h0 = (h0 << 8) + a * (h1 - h0);
        h2 = (h2 << 8) + a * (h3 - h2);
        int c = (h0 << 8) + b * (h2 - h0);

        int y = (((h - hy) * s) >> 11) + 100;

        int la = lasty[i];
        if (y < la) {
            if (lastc[i] == -1) lastc[i] = c;
            int sc = (la != y) ? (c - lastc[i]) / (la - y) : 0;
            int cc = lastc[i];

            if (la > VH - 1) { cc += (la - (VH - 1)) * sc; la = VH - 1; }
            if (y < 0) y = 0;

            while (y < la) {
                int ci = cc >> 16;
                if (ci < 0) ci = 0; if (ci > 255) ci = 255;
                int r = palette[ci * 3] * 4; if (r > 255) r = 255;
                int g = palette[ci * 3 + 1] * 4; if (g > 255) g = 255;
                int bl = palette[ci * 3 + 2] * 4; if (bl > 255) bl = 255;
                if (la >= 0 && la < VH)
                    pixels[la * VW + i] = RGB32(r, g, bl);
                cc += sc; la--;
            }
            lasty[i] = y;
        }
        lastc[i] = c;

        x0 += sx; y0 += sy;
    }
}

void view(Uint32 *pixels, int x0, int y0, double aa, int h) {
    double fov = M_PI / 4.0;
    memset(pixels, 0x33, VW * VH * sizeof(Uint32)); // sky color

    for (int d = 0; d < VW; d++) { lasty[d] = VH; lastc[d] = -1; }

    for (int d = 0; d < 250 - (h < 50) * h; d += 1 + (d >> 6)) {
        draw_line(pixels,
            x0 + (int)(d * 65536.0 * cos(aa - fov)),
            y0 + (int)(d * 65536.0 * sin(aa - fov)),
            x0 + (int)(d * 65536.0 * cos(aa + fov)),
            y0 + (int)(d * 65536.0 * sin(aa + fov)),
            h, 150 * 256 / (d + 1));
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand(42);

    DemoContext *ctx = demo_init("Voxel Terrain - WondY / ZeN", VW, VH);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/ALMA_DEL.XM");
    demo_play_music(music);

    compute_map();

    double a = 0;
    int x0 = 0, y0 = 0, h = 80;
    double ss = 0, sa = 0;
    int spd = 4096;

    while (demo_poll(ctx)) {
        view(ctx->pixels, x0, y0, a, h);
        demo_update(ctx);

        x0 += (int)(3.0 * ss * cos(a));
        y0 += (int)(3.0 * ss * sin(a));
        a += sa;
        sa = 0; ss = 0;

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_LEFT])  sa = -3.0 * 0.05;
        if (keys[SDL_SCANCODE_RIGHT]) sa = 3.0 * 0.05;
        if (keys[SDL_SCANCODE_UP])    ss = 15.0 * spd;
        if (keys[SDL_SCANCODE_DOWN])  ss = -15.0 * spd;
        if (keys[SDL_SCANCODE_Q])     h -= 2;
        if (keys[SDL_SCANCODE_W])     h += 2;
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
