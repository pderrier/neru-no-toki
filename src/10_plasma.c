// SHADEBO2 / Plasma - Original by Wondy 1996
// Interactive heat diffusion with mouse drawing
// Ported from Turbo C++ Mode 13h to SDL2

#include "demo_framework.h"

#define GW 161
#define GH 101

int grid[GW][GH];
unsigned char palette[768];

void arrange(DemoContext *ctx) {
    for (int j = 1; j < GH - 1; j++) {
        for (int i = 1; i < GW - 1; i++) {
            int t = grid[i + 1][j] + grid[i - 1][j]
                  + grid[i + 1][j - 1] + grid[i - 1][j - 1] + grid[i][j - 1]
                  + grid[i + 1][j + 1] + grid[i][j + 1] + grid[i - 1][j + 1];
            t /= 8;
            grid[i - 1][j - 1] = t;
            grid[i + 1][j - 1] = t;
            grid[i][j + 1] = t;
            grid[i][j] = t + 1;

            if (t > 255) t = 255;
            int r = palette[t * 3] * 4; if (r > 255) r = 255;
            int g = palette[t * 3 + 1] * 4; if (g > 255) g = 255;
            int b = palette[t * 3 + 2] * 4; if (b > 255) b = 255;
            Uint32 col = RGB32(r, g, b);
            put_pixel(ctx, i * 2, j * 2, col);
            put_pixel(ctx, i * 2 + 1, j * 2, col);
            put_pixel(ctx, i * 2, j * 2 + 1, col);
            put_pixel(ctx, i * 2 + 1, j * 2 + 1, col);
        }
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Plasma (Draw!) - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/templsun.xm");
    demo_play_music(music);

    // Blue-purple palette (variant from shadebo2)
    memset(palette, 0, 768);
    for (int i = 8; i <= 18; i++)
        palette[i * 3 + 2] = (i - 8) * 63 / 10;
    for (int i = 18; i < 32; i++) {
        palette[i * 3 + 2] = 63 - ((i - 18) * 63 / 14);
        palette[i * 3] = (i - 18) * 63 / 14;
    }
    for (int i = 32; i <= 55; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 2] = (i - 32) * 63 / 23;
    }
    for (int i = 56; i <= 79; i++) {
        palette[i * 3 + 2] = 63;
        palette[i * 3] = 63;
        palette[i * 3 + 1] = (i - 56) * 63 / 23;
    }
    for (int i = 80; i <= 255; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = 63;
        palette[i * 3 + 2] = 63;
    }

    memset(grid, 0, sizeof(grid));

    // Phase 1: Draw with mouse (click)
    // Phase 2: Watch it diffuse (release)

    while (demo_poll(ctx)) {
        int mx, my;
        demo_get_mouse(ctx, &mx, &my);

        if (demo_mouse_pressed()) {
            int gi = mx / 2;
            int gj = my / 2;
            if (gi >= 1 && gi < GW - 1 && gj >= 1 && gj < GH - 1) {
                grid[gi][gj] = 255;
                grid[gi + 1][gj] = 255;
                grid[gi][gj + 1] = 255;
                grid[gi + 1][gj + 1] = 255;
            }
        }

        arrange(ctx);
        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
