// SHADEBOB - Original by Wondy, Juillet 96
// Heat diffusion blob following the mouse
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
            grid[i][j] = t;

            // Draw 2x2 pixel
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

    DemoContext *ctx = demo_init("Shadebob - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/KAOSSONG.MOD");
    demo_play_music(music);

    // Build palette (black -> red -> green -> yellow -> white)
    memset(palette, 0, 768);
    for (int i = 8; i <= 18; i++)
        palette[i * 3] = (i - 8) * 63 / 10;
    for (int i = 18; i < 32; i++) {
        palette[i * 3] = 63 - ((i - 18) * 63 / 14);
        palette[i * 3 + 1] = (i - 18) * 63 / 14;
    }
    for (int i = 32; i <= 55; i++) {
        palette[i * 3 + 1] = 63;
        palette[i * 3] = (i - 32) * 63 / 23;
    }
    for (int i = 56; i <= 79; i++) {
        palette[i * 3 + 1] = 63;
        palette[i * 3] = 63;
        palette[i * 3 + 2] = (i - 56) * 63 / 23;
    }
    for (int i = 80; i <= 255; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = 63;
        palette[i * 3 + 2] = 63;
    }

    memset(grid, 0, sizeof(grid));

    while (demo_poll(ctx)) {
        int mx, my;
        demo_get_mouse(ctx, &mx, &my);
        int gi = mx / 2;
        int gj = my / 2;

        // Clamp
        if (gi < 3) gi = 3;
        if (gj < 3) gj = 3;
        if (gi > GW - 4) gi = GW - 4;
        if (gj > GH - 4) gj = GH - 4;

        // Paint blob around mouse
        for (int dy = -2; dy <= 2; dy++)
            for (int dx = -1; dx <= 1; dx++)
                grid[gi + dx][gj + dy] = 255;

        arrange(ctx);
        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
