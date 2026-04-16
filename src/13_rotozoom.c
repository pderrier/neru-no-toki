// FX2D / Rotozoom - Original by WondY / ZeN (1998)
// Tunnel + rotozoom effect with motion blur
// Ported from Watcom C++ + PTC to SDL2

#include "demo_framework.h"

#define TEX_SIZE 256
#define SW 320
#define SH 240

typedef struct { unsigned char r, g, b; } RGB;

RGB tex[TEX_SIZE * TEX_SIZE];   // texture
unsigned char prof[640 * 480];  // distance table
unsigned tatan2_tab[480 * 640]; // angle table
int _cos1[256], _sin1[256], _cos2[256], _sin2[256];
int _cos3[256], _sin3[256];
double _sin4[256];

void generate_texture(void) {
    for (int y = 0; y < TEX_SIZE; y++) {
        for (int x = 0; x < TEX_SIZE; x++) {
            double fx = (double)x / TEX_SIZE * 4.0 * M_PI;
            double fy = (double)y / TEX_SIZE * 4.0 * M_PI;
            int r = (int)(128.0 + 127.0 * sin(fx) * cos(fy));
            int g = (int)(128.0 + 127.0 * sin(fx + 2.0) * cos(fy + 1.0));
            int b = (int)(128.0 + 127.0 * cos(fx + fy));
            if (r > 255) r = 255; if (r < 0) r = 0;
            if (g > 255) g = 255; if (g < 0) g = 0;
            if (b > 255) b = 255; if (b < 0) b = 0;
            tex[y * TEX_SIZE + x].r = (unsigned char)r;
            tex[y * TEX_SIZE + x].g = (unsigned char)g;
            tex[y * TEX_SIZE + x].b = (unsigned char)b;
        }
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Rotozoom FX2D - WondY / ZeN 1998", SW, SH);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/DISCO.XM");
    demo_play_music(music);

    generate_texture();

    // Precalculate angle and distance tables (original algorithm)
    for (int y = -240; y < 240; y++) {
        for (int x = -320; x < 320; x++) {
            double a = atan2((double)y, (double)x);
            unsigned a2 = (unsigned)(a * 256.0 / M_PI);
            tatan2_tab[(y + 240) * 640 + x + 320] = a2;
            double z = sqrt((double)x * x + (double)y * y);
            prof[(y + 240) * 640 + x + 320] = (unsigned char)z;
        }
    }

    // Movement tables (original values)
    for (int x = 0; x < 256; x++) {
        _cos1[x] = (int)(80.0 * cos((x << 1) * M_PI / 128.0) + 80.0);
        _sin1[x] = (int)(50.0 * sin((x << 2) * M_PI / 128.0) + 175.0);
        _cos2[x] = (int)(70.0 * cos(x * M_PI / 128.0) + 80.0);
        _sin2[x] = (int)(100.0 * sin((x + 10) * M_PI / 128.0) + 175.0);
        _cos3[x] = (int)(70.0 * cos((x + 220) * M_PI / 128.0) + 140.0);
        _sin3[x] = (int)(60.0 * sin((x + 50) * M_PI / 128.0) + 190.0);
        _sin4[x] = sin(x * M_PI / 128.0) + 8.0;
    }

    unsigned xaddi = 0, yaddi = 0;
    unsigned char ccx = 0, ccy = 0;
    int decaly = 0, decalx = 0;

    while (demo_poll(ctx)) {
        ccy += 3;
        ccx += 1;

        int dyang1 = decaly, dxang1 = decalx;
        int dyang2 = _cos1[ccy], dxang2 = _sin1[ccx];
        int dyang3 = _cos2[ccy];
        int dxang3 = _sin2[ccy];
        decaly = _cos3[ccy];
        decalx = _sin3[ccx];

        int totdeca = decaly * 640 + decalx;

        for (int y = 0; y < SH; y++) {
            for (int x = 0; x < SW; x++) {
                unsigned ofs = y * SW + x;

                // Triple angle lookup (original algorithm)
                int idx1 = (y + dyang1) * 640 + x + dxang1;
                int idx2 = (y + dyang2) * 640 + x + dxang2;
                int idx3 = (y + dyang3) * 640 + x + dxang3;

                // Bounds check
                if (idx1 < 0 || idx1 >= 640 * 480 ||
                    idx2 < 0 || idx2 >= 640 * 480 ||
                    idx3 < 0 || idx3 >= 640 * 480) continue;

                unsigned tx = tatan2_tab[idx1] - tatan2_tab[idx2] + tatan2_tab[idx3] + xaddi;

                int profidx = y * 640 + x + totdeca;
                if (profidx < 0 || profidx >= 640 * 480) continue;
                unsigned char ty = prof[profidx];

                int txtofs = ((ty & 0xFF) << 8) + (tx & 0xFF);
                if (txtofs >= TEX_SIZE * TEX_SIZE) txtofs = txtofs % (TEX_SIZE * TEX_SIZE);

                // Motion blur (3/4 old + 1/4 new)
                Uint32 old = ctx->pixels[ofs];
                int or_ = (old >> 16) & 255;
                int og = (old >> 8) & 255;
                int ob = old & 255;

                int r = (tex[txtofs].r + or_ + (or_ << 1)) >> 2;
                int g = (tex[txtofs].g + og + (og << 1)) >> 2;
                int b = (tex[txtofs].b + ob + (ob << 1)) >> 2;

                ctx->pixels[ofs] = RGB32(r, g, b);
            }
        }

        yaddi += (unsigned)_sin4[ccx];
        xaddi--;

        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
