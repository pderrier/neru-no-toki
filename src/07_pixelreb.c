// PIXELREB - Original by Wondy 1996
// Bouncing particles on terrain with gravity
// Ported from Turbo C++ Mode 13h to SDL2

#include "demo_framework.h"

#define NB_POINTS 55

int height_map[320]; // ground height at each column

void bresenham_line(DemoContext *ctx, int x1, int y1, int x2, int y2, Uint32 col) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        put_pixel(ctx, x1, y1, col);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void init_ground_segment(int x1, int y1, int x2, int y2) {
    if (x1 > x2) { int t; t=x1; x1=x2; x2=t; t=y1; y1=y2; y2=t; }
    int dx = x2 - x1;
    if (dx == 0) return;
    for (int x = x1; x <= x2 && x < 320; x++) {
        int y = y1 + (y2 - y1) * (x - x1) / dx;
        if (x >= 0 && x < 320 && y < height_map[x])
            height_map[x] = y;
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    DemoContext *ctx = demo_init("PixelReb - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/songsect.xm");
    demo_play_music(music);

    // Init ground
    for (int i = 0; i < 320; i++) height_map[i] = 200;
    init_ground_segment(0, 193, 320, 193);
    init_ground_segment(0, 150, 200, 193);
    init_ground_segment(200, 75, 320, 0);

    // Init particles
    int px[NB_POINTS], py[NB_POINTS], pc[NB_POINTS];
    int diry[NB_POINTS], dirx[NB_POINTS];

    for (int i = 0; i < NB_POINTS; i++) {
        px[i] = rand() % 320;
        py[i] = rand() % 180;
        while (py[i] > height_map[px[i]]) py[i] = rand() % 180;
        diry[i] = rand() % 4 + 1;
        dirx[i] = rand() % 3 - 1;
        pc[i] = rand() % 15 + 31;
    }

    while (demo_poll(ctx)) {
        // Clear
        memset(ctx->pixels, 0, 320 * 200 * sizeof(Uint32));

        // Draw ground lines
        Uint32 white = RGB32(255, 255, 255);
        bresenham_line(ctx, 0, 193, 319, 193, white);
        bresenham_line(ctx, 0, 150, 200, 193, white);
        bresenham_line(ctx, 200, 75, 319, 0, white);

        // Update particles
        for (int i = 0; i < NB_POINTS; i++) {
            if (py[i] > height_map[px[i] < 320 ? px[i] : 319])
                diry[i] = -abs(diry[i]);
            diry[i]++;
            py[i] += diry[i];
            py[i]++;
            px[i] += dirx[i];
            if (px[i] <= 0) dirx[i] = abs(dirx[i]);
            if (px[i] >= 319) dirx[i] = -abs(dirx[i]);
        }

        // Draw particles
        for (int i = 0; i < NB_POINTS; i++) {
            int xi = px[i], yi = py[i];
            if (yi > 0 && xi >= 0 && xi < 320 && yi < height_map[xi]) {
                int brightness = pc[i] * 4;
                if (brightness > 255) brightness = 255;
                Uint32 col = RGB32(brightness, brightness / 2, brightness / 3);
                put_pixel(ctx, xi, yi, col);
            }
        }

        demo_update(ctx);
        SDL_Delay(16);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
