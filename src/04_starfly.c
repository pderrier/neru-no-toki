// STARFLY (3D Starfield) - Original by Wondy 10/96
// Ported from Turbo C++ Mode X to SDL2

#include "demo_framework.h"

#define NBETOILES 100
#define MAX_STARS 200

int sx[MAX_STARS], sy[MAX_STARS], sz[MAX_STARS];

void draw_star(DemoContext *ctx, int px, int py, int brightness) {
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    Uint32 col = RGB32(brightness, brightness, brightness);

    if (px >= 0 && px < 320 && py >= 0 && py < 200)
        ctx->pixels[py * 320 + px] = col;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    DemoContext *ctx = demo_init("Starfly - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/songsect.xm");
    demo_play_music(music);

    // Init stars
    for (int i = 0; i < NBETOILES; i++) {
        sx[i] = rand() % 2000 - 1000;
        sy[i] = rand() % 2000 - 1000;
        sz[i] = rand() % 1000 + 1;
    }

    int speed = 20;
    int movex = 0, movey = 0;

    while (demo_poll(ctx)) {
        // Clear screen
        memset(ctx->pixels, 0, 320 * 200 * sizeof(Uint32));

        // Draw stars
        for (int i = 0; i < NBETOILES; i++) {
            int x0 = (sx[i] * 128) / sz[i] + 160;
            int y0 = (sy[i] * 128) / sz[i] + 100;

            if (x0 >= 0 && x0 < 320 && y0 >= 0 && y0 < 200) {
                double r = sqrt((double)sx[i] * sx[i] + (double)sy[i] * sy[i] + (double)sz[i] * sz[i]);
                int brightness = 255 - (int)(r / 6);
                if (brightness < 30) brightness = 30;

                if (r < 750) {
                    // Big star (cross shape)
                    draw_star(ctx, x0, y0 - 1, brightness);
                    draw_star(ctx, x0 - 1, y0, brightness);
                    draw_star(ctx, x0, y0, brightness);
                    draw_star(ctx, x0 + 1, y0, brightness);
                    draw_star(ctx, x0, y0 + 1, brightness);
                } else if (r < 850) {
                    // Medium star
                    draw_star(ctx, x0, y0, brightness);
                    draw_star(ctx, x0 + 1, y0, brightness);
                } else {
                    // Small star
                    draw_star(ctx, x0, y0, brightness);
                }
            }
        }

        demo_update(ctx);

        // Handle input
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        movex = 0; movey = 0;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) movex = -15;
        if (keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) movex = 15;
        if (keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) movey = 15;
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) movey = -15;
        if (keys[SDL_SCANCODE_KP_PLUS] || keys[SDL_SCANCODE_EQUALS]) speed++;
        if (keys[SDL_SCANCODE_KP_MINUS] || keys[SDL_SCANCODE_MINUS]) speed--;

        // Update stars
        for (int i = 0; i < NBETOILES; i++) {
            sx[i] += movex;
            sy[i] += movey;
            sz[i] -= speed;
            if (sz[i] < 10) {
                sz[i] = 1000;
                sx[i] = rand() % 2000 - 1000;
                sy[i] = rand() % 2000 - 1000;
            }
            if (sz[i] > 1000) {
                sz[i] = 10;
                sx[i] = rand() % 2000 - 1000;
                sy[i] = rand() % 2000 - 1000;
            }
        }
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
