// FRAKTAL - Original by Wondy, Dec 96
// Mandelbrot set with palette rotation
// Ported from Turbo C++ Mode 13h to SDL2

#include "demo_framework.h"

unsigned char screen_buf[320 * 200];
unsigned char palette[768];
unsigned char base_palette[768];

void palrot(int max) {
    unsigned char r = palette[max * 3];
    unsigned char g = palette[max * 3 + 1];
    unsigned char b = palette[max * 3 + 2];
    memmove(&palette[3], &palette[0], max * 3);
    palette[0] = r;
    palette[1] = g;
    palette[2] = b;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Fraktal - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/KAOSSONG.MOD");
    demo_play_music(music);

    // Generate a fire-like palette
    palette[0] = 0; palette[1] = 0; palette[2] = 0;
    for (int i = 1; i < 64; i++) {
        palette[i * 3] = i;           // red
        palette[i * 3 + 1] = 0;
        palette[i * 3 + 2] = 0;
    }
    for (int i = 64; i < 128; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = (i - 64);
        palette[i * 3 + 2] = 0;
    }
    for (int i = 128; i < 192; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = 63;
        palette[i * 3 + 2] = (i - 128);
    }
    for (int i = 192; i < 256; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = 63;
        palette[i * 3 + 2] = 63;
    }
    memcpy(base_palette, palette, 768);

    // Compute Mandelbrot (original algorithm)
    for (int i = -100; i < 101; i++) {
        for (int j = -500; j < 500; j++) {
            double n1 = 0, n2 = 0, x = 0, y = 0;
            int k;
            for (k = 0; k < 16; k++) {
                n1 = x; n2 = y;
                x = n1 * n1 - n2 * n2 + (double)i / 100.0;
                y = 2 * n1 * n2 + (double)j / 250.0;
                double rac = sqrt(x * x + y * y);
                int px = (j + 500) / 3;
                int py = i + 100;
                if (px >= 0 && px < 320 && py >= 0 && py < 200)
                    screen_buf[py * 320 + px] = (unsigned char)rac;
                if (rac > 50) break;
            }
        }
    }

    // Render initial frame
    for (int i = 0; i < 320 * 200; i++) {
        unsigned char c = screen_buf[i];
        ctx->pixels[i] = RGB32(palette[c * 3] * 4, palette[c * 3 + 1] * 4, palette[c * 3 + 2] * 4);
    }
    demo_update(ctx);

    // Palette rotation loop
    while (demo_poll(ctx)) {
        palrot(255);
        palrot(255);

        for (int i = 0; i < 320 * 200; i++) {
            unsigned char c = screen_buf[i];
            int r = palette[c * 3] * 4; if (r > 255) r = 255;
            int g = palette[c * 3 + 1] * 4; if (g > 255) g = 255;
            int b = palette[c * 3 + 2] * 4; if (b > 255) b = 255;
            ctx->pixels[i] = RGB32(r, g, b);
        }

        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
