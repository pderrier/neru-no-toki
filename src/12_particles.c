// PTCUL - Original by WondY / ZeN
// Particle system with additive blending
// Ported from DJGPP Mode 13h to SDL2

#include "demo_framework.h"

#define NB_PARTICULES 200
#define SPRITE_SIZE 16

typedef struct {
    float xp, yp;
    float v_dep;
    int vie;
    float vx, vy;
} Particle;

// Sprite brightness table [150 levels][16x16 pixels]
unsigned char sprite[150][SPRITE_SIZE * SPRITE_SIZE];
unsigned char screen_buf[320 * 200]; // 8-bit additive buffer

int CENTREX = 160, CENTREY = 100;
int addcx = 1, addcy = 1;

void normalise(Particle *p) {
    float norme = sqrtf(p->vx * p->vx + p->vy * p->vy);
    if (norme > 0.001f) { p->vx /= norme; p->vy /= norme; }
}

void reinit_particle(Particle *p) {
    p->xp = (float)CENTREX;
    p->yp = (float)CENTREY;
    p->v_dep = (float)(rand() % 400) / 110.0f;
    p->vie = (rand() % 150) + 100;
    if (CENTREX < 75)       p->vx = (float)((rand() % 4) + 4);
    else if (CENTREX < 100) p->vx = (float)((rand() % 4) - 2);
    else                    p->vx = (float)((rand() % 4) - 8);
    p->vy = 10.0f;
    normalise(p);
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    DemoContext *ctx = demo_init("Particles - WondY / ZeN", SCREEN_W, SCREEN_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/DISCO.XM");
    demo_play_music(music);

    // Generate sprite (radial gradient)
    for (int lum = 0; lum < 150; lum++) {
        for (int y = 0; y < SPRITE_SIZE; y++) {
            for (int x = 0; x < SPRITE_SIZE; x++) {
                float dx = (float)(x - SPRITE_SIZE / 2);
                float dy = (float)(y - SPRITE_SIZE / 2);
                float dist = sqrtf(dx * dx + dy * dy);
                int val = (int)(255.0f * (1.0f - dist / (SPRITE_SIZE / 2.0f)));
                val = val * (150 - lum) / 150;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                sprite[lum][y * SPRITE_SIZE + x] = (unsigned char)val;
            }
        }
    }

    Particle particles[NB_PARTICULES];
    int alive[NB_PARTICULES];
    memset(alive, 0, sizeof(alive));

    for (int i = 0; i < NB_PARTICULES; i++) {
        alive[i] = 0;
        reinit_particle(&particles[i]);
    }

    while (demo_poll(ctx)) {
        // Fade screen buffer
        for (int i = 0; i < 320 * 200; i++) {
            screen_buf[i] >>= 1;
        }

        // Move emitter
        CENTREX += addcx;
        CENTREY += addcy;
        if (CENTREX < 10 || CENTREX > 299) addcx = -addcx;
        if (CENTREY < 70 || CENTREY > 178) addcy = -addcy;

        // Create new particles
        int nb_new = rand() % 10;
        for (int j = 0; j < nb_new; j++) {
            for (int i = 0; i < NB_PARTICULES; i++) {
                if (!alive[i]) {
                    alive[i] = 1;
                    reinit_particle(&particles[i]);
                    break;
                }
            }
        }

        // Update and render particles
        for (int j = 0; j < NB_PARTICULES; j++) {
            if (!alive[j]) continue;

            int lum = 150 - (particles[j].vie - 100);
            if (lum < 0) lum = 0;
            if (lum >= 150) lum = 149;

            int px = (int)particles[j].xp;
            int py = (int)particles[j].yp;

            // Draw sprite with additive blend
            for (int sy = 0; sy < SPRITE_SIZE; sy++) {
                for (int sx = 0; sx < SPRITE_SIZE; sx++) {
                    int dx = px + sx;
                    int dy = py + sy;
                    if (dx >= 0 && dx < 320 && dy >= 0 && dy < 200) {
                        int idx = dy * 320 + dx;
                        int val = screen_buf[idx] + sprite[lum][sy * SPRITE_SIZE + sx];
                        if (val > 255) val = 255;
                        screen_buf[idx] = (unsigned char)val;
                    }
                }
            }

            // Evolve particle
            particles[j].xp += particles[j].vx;
            particles[j].yp -= particles[j].vy * particles[j].v_dep;
            particles[j].vie--;
            particles[j].v_dep -= 0.03f;
            if (particles[j].vie <= 0) alive[j] = 0;
            if (particles[j].yp < 5 || particles[j].yp > 178) alive[j] = 0;
        }

        // Render to screen (fire palette)
        for (int i = 0; i < 320 * 200; i++) {
            unsigned char v = screen_buf[i];
            int r = v;
            int g = v * 3 / 4;
            int b = v / 3;
            ctx->pixels[i] = RGB32(r, g, b);
        }

        demo_update(ctx);
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
