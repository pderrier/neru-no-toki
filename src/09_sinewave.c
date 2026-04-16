// ESSSIN + RUBBER - Original by Wondy 1996
// Sine wave horizontal bars effect
// Ported from Turbo C++ Mode 13h to SDL2

#include "demo_framework.h"

int stab1[256];
int stab2[256];

#define AMP1 20
#define AMP2 18
#define SPD1 1
#define SPD2 5
#define SLEN1 250
#define SLEN2 180

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Sine Waves - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/DISCO.XM");
    demo_play_music(music);

    // Precalc sine tables
    for (int j = 0; j < SLEN1; j++)
        stab1[j] = (int)(sin(2.0 * j * M_PI / SLEN1) * AMP1);
    for (int j = 0; j < SLEN2; j++)
        stab2[j] = (int)(cos(2.0 * j * M_PI / SLEN2) * AMP2);

    // Also a second set for the "rubber" effect
    int tabl1[256], tabl2[256];
    for (int i = 0; i < 256; i++) {
        tabl1[i] = (int)(sin(2.0 * i * M_PI / 128.0) * 32.0 + 16.0);
        tabl2[i] = (int)(sin(2.0 * i * M_PI / 256.0) * 32.0 + 16.0);
    }

    unsigned inc1 = 0, inc2 = 0;
    unsigned char compt = 0, compt2 = 0;
    int mode = 0; // 0 = sine, 1 = rubber

    while (demo_poll(ctx)) {
        memset(ctx->pixels, 0, 320 * 200 * sizeof(Uint32));

        if (mode == 0) {
            // ESSSIN mode: double sine wave bars
            for (int y = 0; y < 200; y++) {
                int C = 45 + stab1[(y + inc1) % SLEN1] + stab2[(y + inc2) % SLEN2];
                if (C < 0) C = 0;

                // Draw colored bar
                int brightness = y;
                int r = brightness;
                int g = brightness / 2;
                int b = 255 - brightness;
                for (int x = 0; x < C && x < 320; x++)
                    ctx->pixels[y * 320 + x] = RGB32(r, g, b);
            }
            inc2 += SPD2;
            inc1 += SPD1;
        } else {
            // RUBBER mode: nested sine distortion
            for (int y = 0; y < 200; y++) {
                int temp = tabl1[(tabl2[(y + compt2) & 255] + compt) & 255];
                if (temp < 0) temp = 0;

                int brightness = y;
                int r = brightness;
                int g = 100;
                int b = 255 - brightness;
                for (int x = 0; x < temp && x < 320; x++)
                    ctx->pixels[y * 320 + x] = RGB32(r, g, b);
            }
            compt++;
            compt2 += 3;
        }

        demo_update(ctx);

        // Space to switch mode
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_SPACE]) { mode = 1 - mode; SDL_Delay(200); }
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
