// CHAOS - Original by WondY / ZeN
// Bifurcation diagram (logistic map)
// Ported from Watcom C++ Mode 13h to SDL2

#include "demo_framework.h"

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Chaos (Bifurcation) - WondY / ZeN", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/songsect.xm");
    demo_play_music(music);

    unsigned char palette[768];
    palette[0] = 0; palette[1] = 0; palette[2] = 0;
    for (int c = 3; c < 768; c += 3) {
        palette[c] = c / 12;
        palette[c + 1] = c / 12;
        palette[c + 2] = c / 12;
    }

    unsigned char buffer[320 * 200];
    memset(buffer, 0, sizeof(buffer));

    unsigned n = 64;      // effective complexity (can still be tweaked with +/-)
    float x0 = 0.0f;
    unsigned frame = 0;

    while (demo_poll(ctx)) {
        // Gentle persistence/fade to keep trails while avoiding full white-out.
        if ((frame & 3u) == 0) {
            for (int i = 0; i < 320 * 200; i++) {
                if (buffer[i] > 0) buffer[i]--;
            }
        }

        // Auto-ramp detail for a denser image during startup.
        if ((frame % 20u) == 0u && n < 192) n++;

        // Compute bifurcation diagram (standalone logistic-map mode).
        for (int px = 0; px < 320; px++) {
            float a = (4.0f * (float)px) / 319.0f;
            float y = x0 + (float)px * 0.003137f;
            while (y >= 1.0f) y -= 1.0f;

            unsigned transient = n * 8u + 96u;
            for (unsigned c = 0; c < transient; c++) {
                y = a * y * (1.0f - y);
            }

            unsigned samples = (n >> 2) + 16u;
            for (unsigned c = 0; c < samples; c++) {
                y = a * y * (1.0f - y);
                int py = 199 - (int)(y * 199.0f);
                if (py >= 0 && py < 200) {
                    int idx = py * 320 + px;
                    unsigned int v = (unsigned int)buffer[idx] + 12u;
                    buffer[idx] = (unsigned char)(v > 255u ? 255u : v);
                }
            }
        }

        // Render
        for (int i = 0; i < 320 * 200; i++) {
            unsigned char idx = buffer[i];
            int r = palette[idx * 3] * 4; if (r > 255) r = 255;
            int g = palette[idx * 3 + 1] * 4; if (g > 255) g = 255;
            int b = palette[idx * 3 + 2] * 4; if (b > 255) b = 255;
            ctx->pixels[i] = RGB32(r, g, b);
        }
        demo_update(ctx);

        x0 += 0.007f;
        if (x0 >= 1.0f) x0 -= 1.0f;

        frame++;

        // Handle +/- to change iteration count
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_KP_PLUS] || keys[SDL_SCANCODE_EQUALS]) { if (n < 512) n++; SDL_Delay(100); }
        if (keys[SDL_SCANCODE_KP_MINUS] || keys[SDL_SCANCODE_MINUS]) { if (n > 8) n--; SDL_Delay(100); }
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
