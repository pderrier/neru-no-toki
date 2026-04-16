// TUNNEL 3D - Original by Wondy, Novembre 1996
// Textured tunnel using atan2/distance precalc tables
// Ported from Turbo C++ Mode 13h to SDL2

#include "demo_framework.h"

#define TW 320
#define TH 200
#define TEX_SIZE 128

unsigned char angles[TH * TW];
unsigned char depth_map[TH * TW];
unsigned char texture[TEX_SIZE * TEX_SIZE];
unsigned char palette[768];

void generate_texture(void) {
    // Generate a procedural texture since we don't have the original PCX
    for (int y = 0; y < TEX_SIZE; y++) {
        for (int x = 0; x < TEX_SIZE; x++) {
            int v = (int)(128.0 + 64.0 * sin(x * M_PI / 16.0) * cos(y * M_PI / 16.0));
            v += (x ^ y) & 31;
            if (v > 255) v = 255;
            if (v < 0) v = 0;
            texture[y * TEX_SIZE + x] = (unsigned char)v;
        }
    }
}

void generate_palette(void) {
    for (int i = 0; i < 256; i++) {
        // Copper/amber palette
        palette[i * 3]     = (unsigned char)(i * 63 / 255);  // R
        palette[i * 3 + 1] = (unsigned char)(i * 40 / 255);  // G
        palette[i * 3 + 2] = (unsigned char)(i * 20 / 255);  // B
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("Tunnel 3D - Wondy 1996", TW, TH);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/DISCO.XM");
    demo_play_music(music);

    generate_texture();
    generate_palette();

    const double R = 100.0;
    const double D = 50.0;
    int nbmap = 2; // texture mapping repeat

    // Precalculate angle and depth tables (original algorithm)
    for (int y = -100; y < 100; y++) {
        for (int x = -40; x < 280; x++) {
            int idx = (y + 100) * TW + (x + 40);
            if (idx < 0 || idx >= TW * TH) continue;
            if (x != 0) {
                double a = atan2((double)y, (double)x);
                unsigned char z = (unsigned char)(fabs(cos(a) * R * D / (double)x));
                unsigned char a2 = (unsigned char)(a * ((TEX_SIZE * nbmap) / 2) / M_PI);
                angles[idx] = a2;
                depth_map[idx] = z;
            }
        }
        // Fix center column
        int idx1 = (y + 100) * TW + 200; // x=160 -> +40=200
        int idx2 = (y + 100) * TW + 199;
        if (idx1 >= 0 && idx1 < TW * TH && idx2 >= 0 && idx2 < TW * TH) {
            angles[idx1] = angles[idx2];
            depth_map[idx1] = depth_map[idx2];
        }
    }

    unsigned x_adder = 0;
    unsigned y_adder = 0;

    while (demo_poll(ctx)) {
        // Render tunnel (symmetric from center)
        for (int i = 0; i < TW * TH / 2; i++) {
            int mirror = TW * TH - 1 - i;
            unsigned char tx = angles[i] + (unsigned char)x_adder;
            unsigned char ty = depth_map[i] + (unsigned char)y_adder;
            int tex_idx = ((ty & (TEX_SIZE - 1)) * TEX_SIZE) + (tx & (TEX_SIZE - 1));
            unsigned char c = texture[tex_idx];

            int r = palette[c * 3] * 4; if (r > 255) r = 255;
            int g = palette[c * 3 + 1] * 4; if (g > 255) g = 255;
            int b = palette[c * 3 + 2] * 4; if (b > 255) b = 255;
            Uint32 col = RGB32(r, g, b);

            ctx->pixels[i] = col;
            if (mirror >= 0 && mirror < TW * TH)
                ctx->pixels[mirror] = col;
        }

        x_adder += 3;
        y_adder += 8;

        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
