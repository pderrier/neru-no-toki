// FLAMMES - Original by Wondy 08/96
// Ported from Turbo Pascal + Mode 13h to SDL2
// Fire effect with scrolling interpolation

#include "demo_framework.h"

#define FW 320
#define FH 100

unsigned char source[FH][FW];
unsigned char dest[FH][FW];
unsigned char palette[768];

void prep_pal(void) {
    memset(palette, 0, 80 * 3);
    // Blue fade in/out
    for (int i = 0; i < 4; i++) {
        palette[i * 3 + 2] = i * 4;
        palette[(i + 4) * 3 + 2] = 16 - i * 4;
    }
    // Red increasing (8-19)
    for (int i = 8; i <= 19; i++)
        palette[i * 3] = (i - 8) * 63 / 11;
    // Green increasing, red constant (19-30)
    for (int i = 19; i <= 30; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = (i - 19) * 63 / 11;
    }
    // Blue increasing, red+green constant (31-42)
    for (int i = 31; i <= 42; i++) {
        palette[i * 3] = 63;
        palette[i * 3 + 1] = 63;
        palette[i * 3 + 2] = (i - 31) * 63 / 11;
    }
    // Rest = white
    memset(&palette[42 * 3], 63, 86 * 3);

    // Mirror palette (for the inverted display)
    for (int i = 129; i < 133; i++) palette[i * 3 + 2] = (i - 129) * 4;
    for (int i = 133; i < 137; i++) palette[i * 3 + 2] = 16 - (i - 133) * 4;
    for (int i = 137; i <= 148; i++) palette[i * 3 + 2] = (i - 137) * 63 / 11;
    for (int i = 148; i <= 159; i++) {
        palette[i * 3 + 2] = 63;
        palette[i * 3 + 1] = (i - 148) * 63 / 11;
    }
    for (int i = 160; i <= 171; i++) {
        palette[i * 3 + 2] = 63;
        palette[i * 3 + 1] = 63;
        palette[i * 3] = (i - 160) * 63 / 11;
    }
    memset(&palette[171 * 3], 63, 85 * 3);
}

void scrolling(void) {
    for (int y = 1; y < FH - 1; y++) {
        for (int x = 1; x < FW - 1; x++) {
            int sum = source[y - 1][x - 1] + source[y - 1][x] + source[y - 1][x + 1]
                    + source[y][x - 1] + source[y][x + 1]
                    + source[y + 1][x - 1] + source[y + 1][x] + source[y + 1][x + 1];
            sum >>= 3;
            if (sum > 0) sum--;
            dest[y - 1][x] = (unsigned char)sum;
        }
    }
}

void new_lines(void) {
    for (int x = 0; x < FW; x++) {
        dest[97][x] = rand() % 7 + 35;
        dest[98][x] = rand() % 7 + 35;
        dest[99][x] = rand() % 7 + 35;
    }
    int nb_foyers = rand() % 45;
    for (int i = 0; i < nb_foyers; i++) {
        int x = rand() % (FW - 2) + 1;
        int y = 98;
        if (y > 0 && y < FH - 1 && x > 0 && x < FW - 1) {
            dest[y - 1][x - 1] = 0xA0; dest[y - 1][x] = 0xA0; dest[y - 1][x + 1] = 0xA0;
            dest[y][x - 1] = 0xA0;     dest[y][x] = 0xA0;     dest[y][x + 1] = 0xA0;
            dest[y + 1][x - 1] = 0xA0; dest[y + 1][x] = 0xA0; dest[y + 1][x + 1] = 0xA0;
        }
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    DemoContext *ctx = demo_init("Flammes - Wondy 1996", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/KAOSSONG.MOD");
    demo_play_music(music);

    prep_pal();
    memset(source, 0, sizeof(source));
    memset(dest, 0, sizeof(dest));

    while (demo_poll(ctx)) {
        scrolling();
        new_lines();

        // Display: normal fire top half, mirrored bottom half
        for (int y = 0; y < FH; y++) {
            for (int x = 0; x < FW; x++) {
                unsigned char c = dest[y][x];
                int r = palette[c * 3] * 4;
                int g = palette[c * 3 + 1] * 4;
                int b = palette[c * 3 + 2] * 4;
                if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
                // Top half (normal)
                ctx->pixels[y * FW + x] = RGB32(r, g, b);
                // Bottom half (mirrored)
                int my = SCREEN_H - 1 - y;
                if (my >= FH)
                    ctx->pixels[my * FW + x] = RGB32(r, g, b);
            }
        }

        // Swap source/dest
        unsigned char (*tmp)[FW] = (void*)malloc(sizeof(source));
        memcpy(tmp, dest, sizeof(source));
        memcpy(source, dest, sizeof(source));
        memcpy(dest, tmp, sizeof(source));
        free(tmp);

        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
