// WOBBLER (WOB) - Original by Wondy 1996
// Image distortion using sine tables
// Ported from Turbo C++ Mode 13h to SDL2

#include "demo_framework.h"

#define IMG_W 320
#define IMG_H 200
#define MIN_X 5
#define MIN_Y 5
#define MAX_X 315
#define MAX_Y 195

unsigned char image[IMG_W * IMG_H];
unsigned char palette[768];
int sintbl[101];

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Wobbler - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/templsun.xm");
    demo_play_music(music);

    // Precalculate sine table
    for (int i = 0; i < 76; i++) {
        int idx = i * 63 / 75;
        if (idx < 101)
            sintbl[idx] = (int)((sin((double)i * 20.0 / M_PI) + 1.0) * 5.0);
    }
    // Fill gaps in table
    for (int i = 1; i < 64; i++) {
        if (sintbl[i] == 0 && sintbl[i-1] != 0)
            sintbl[i] = sintbl[i-1];
    }

    // Generate a procedural image (checkerboard with gradient)
    memset(palette, 0, 768);
    for (int i = 0; i < 256; i++) {
        palette[i * 3] = (unsigned char)(32 + (i * 31) / 255);
        palette[i * 3 + 1] = (unsigned char)(16 + (i * 47) / 255);
        palette[i * 3 + 2] = (unsigned char)(48 + (i * 15) / 255);
    }

    for (int y = 0; y < IMG_H; y++) {
        for (int x = 0; x < IMG_W; x++) {
            int cx = x - 160, cy = y - 100;
            double dist = sqrt(cx * cx + cy * cy);
            double angle = atan2(cy, cx);
            int v = (int)(128 + 64 * sin(angle * 4) * cos(dist / 20.0));
            if (v < 0) v = 0; if (v > 255) v = 255;
            if (x >= MIN_X && x < MAX_X && y >= MIN_Y && y < MAX_Y)
                image[y * IMG_W + x] = (unsigned char)v;
            else
                image[y * IMG_W + x] = 0;
        }
    }

    int j = 0;

    while (demo_poll(ctx)) {
        j = (j + 3) & 63;

        for (int y = MIN_Y; y < MAX_Y; y++) {
            int val_y = sintbl[(y + j) & 31];
            for (int x = MIN_X; x < MAX_X; x += 2) {
                int val_x = sintbl[(x + j) & 63];
                int src_y = y + val_x;
                int src_x = x + val_y;

                // Bounds check
                if (src_y < 0) src_y = 0;
                if (src_y >= IMG_H) src_y = IMG_H - 1;
                if (src_x < 0) src_x = 0;
                if (src_x >= IMG_W - 1) src_x = IMG_W - 2;

                unsigned char c1 = image[src_y * IMG_W + src_x];
                unsigned char c2 = image[src_y * IMG_W + src_x + 1];

                int r, g, b;
                r = palette[c1 * 3] * 4; if (r > 255) r = 255;
                g = palette[c1 * 3 + 1] * 4; if (g > 255) g = 255;
                b = palette[c1 * 3 + 2] * 4; if (b > 255) b = 255;
                ctx->pixels[y * IMG_W + x] = RGB32(r, g, b);

                r = palette[c2 * 3] * 4; if (r > 255) r = 255;
                g = palette[c2 * 3 + 1] * 4; if (g > 255) g = 255;
                b = palette[c2 * 3 + 2] * 4; if (b > 255) b = 255;
                ctx->pixels[y * IMG_W + x + 1] = RGB32(r, g, b);
            }
        }

        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
