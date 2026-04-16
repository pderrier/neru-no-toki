// W3D MOTOR - Original by WondY / ZeN (1998)
// Software 3D engine: flat shading, rotation, z-sort
// Ported from Watcom C + Mode 13h to SDL2
// (Simplified to flat-shaded wireframe/filled triangles)

#include "demo_framework.h"

#define MAX_SOM 1024
#define MAX_FACES 2048
#define SW 320
#define SH 200

typedef struct {
    int x, y, z;       // original coords
    int rx, ry, rz;    // rotated coords
    int xp, yp;        // projected screen coords
    int rnx, rny, rnz; // rotated normals
} Vertex;

typedef struct {
    int v1, v2, v3;
    int rnz; // face normal z
} Face;

Vertex verts[MAX_SOM];
Face faces[MAX_FACES];
int nb_som = 0, nb_faces = 0;
int ADDZ = 500;

unsigned char buffer[SW * SH];
unsigned char palette[768];

// Sine/cosine tables (fixed point 10-bit)
#define FP_SHIFT 10
#define FP_ONE (1 << FP_SHIFT)
int sin_tab[256], cos_tab[256];

void init_trig(void) {
    for (int i = 0; i < 256; i++) {
        sin_tab[i] = (int)(sin(i * 2.0 * M_PI / 256.0) * FP_ONE);
        cos_tab[i] = (int)(cos(i * 2.0 * M_PI / 256.0) * FP_ONE);
    }
}

void generate_object(void) {
    // Generate a torus
    int rings = 16, sides = 12;
    double R = 80, r = 30;

    nb_som = 0;
    for (int i = 0; i < rings; i++) {
        double theta = 2.0 * M_PI * i / rings;
        for (int j = 0; j < sides; j++) {
            double phi = 2.0 * M_PI * j / sides;
            verts[nb_som].x = (int)((R + r * cos(phi)) * cos(theta));
            verts[nb_som].y = (int)((R + r * cos(phi)) * sin(theta));
            verts[nb_som].z = (int)(r * sin(phi));
            nb_som++;
        }
    }

    nb_faces = 0;
    for (int i = 0; i < rings; i++) {
        int ni = (i + 1) % rings;
        for (int j = 0; j < sides; j++) {
            int nj = (j + 1) % sides;
            faces[nb_faces].v1 = i * sides + j;
            faces[nb_faces].v2 = ni * sides + j;
            faces[nb_faces].v3 = i * sides + nj;
            nb_faces++;
            faces[nb_faces].v1 = ni * sides + j;
            faces[nb_faces].v2 = ni * sides + nj;
            faces[nb_faces].v3 = i * sides + nj;
            nb_faces++;
        }
    }
}

void rotate_project(int angx, int angy, int angz) {
    int cx = cos_tab[angx & 255], sx = sin_tab[angx & 255];
    int cy = cos_tab[angy & 255], sy = sin_tab[angy & 255];
    int cz = cos_tab[angz & 255], sz = sin_tab[angz & 255];

    for (int i = 0; i < nb_som; i++) {
        int x = verts[i].x, y = verts[i].y, z = verts[i].z;

        // Rotate Y
        int x1 = (x * cy + z * sy) >> FP_SHIFT;
        int z1 = (-x * sy + z * cy) >> FP_SHIFT;
        // Rotate X
        int y1 = (y * cx - z1 * sx) >> FP_SHIFT;
        int z2 = (y * sx + z1 * cx) >> FP_SHIFT;
        // Rotate Z
        int x2 = (x1 * cz - y1 * sz) >> FP_SHIFT;
        int y2 = (x1 * sz + y1 * cz) >> FP_SHIFT;

        verts[i].rx = x2;
        verts[i].ry = y2;
        verts[i].rz = z2;

        // Project
        int zp = z2 + ADDZ;
        if (zp < 10) zp = 10;
        verts[i].xp = (x2 * 256 / zp) + SW / 2;
        verts[i].yp = (y2 * 256 / zp) + SH / 2;
    }
}

// Scanline-fill a flat-shaded triangle into the 8-bit buffer
void fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, unsigned char color) {
    // Sort by y
    if (y0 > y1) { int t; t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }
    if (y0 > y2) { int t; t=x0;x0=x2;x2=t; t=y0;y0=y2;y2=t; }
    if (y1 > y2) { int t; t=x1;x1=x2;x2=t; t=y1;y1=y2;y2=t; }

    if (y2 == y0) return;

    for (int y = y0; y <= y2; y++) {
        if (y < 0 || y >= SH) continue;
        int xa, xb;

        // Interpolate edges
        if (y2 != y0)
            xa = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        else
            xa = x0;

        if (y < y1) {
            if (y1 != y0) xb = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
            else xb = x0;
        } else {
            if (y2 != y1) xb = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            else xb = x1;
        }

        if (xa > xb) { int t = xa; xa = xb; xb = t; }
        if (xa < 0) xa = 0;
        if (xb >= SW) xb = SW - 1;

        for (int x = xa; x <= xb; x++)
            buffer[y * SW + x] = color;
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    DemoContext *ctx = demo_init("W3D Motor - WondY / ZeN 1998", SW, SH);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/EXEL.XM");
    demo_play_music(music);

    init_trig();
    generate_object();

    // Generate palette (grayscale for flat shading)
    for (int i = 0; i < 256; i++) {
        palette[i * 3] = (unsigned char)(i / 4);
        palette[i * 3 + 1] = (unsigned char)(i / 4);
        palette[i * 3 + 2] = (unsigned char)(i / 4);
    }

    int angx = 0, angy = 0, angz = 0;
    int motion_blur = 0;
    int auto_rot = 1;

    while (demo_poll(ctx)) {
        // Clear or blur
        if (!motion_blur) {
            memset(buffer, 0, sizeof(buffer));
        } else {
            // Flame-like blur
            for (int j = SW; j < SW * (SH - 1); j++) {
                int a = ((int)buffer[j - 1] + buffer[j - SW] + buffer[j + 1] + buffer[j + SW]) >> 2;
                if (a > 1) buffer[j - SW] = (unsigned char)(a - 2);
                else buffer[j - SW] = 0;
            }
        }

        rotate_project(angx, angy, angz);

        // Z-sort faces (simple insertion sort by average z)
        int order[MAX_FACES];
        int avgz[MAX_FACES];
        for (int i = 0; i < nb_faces; i++) {
            order[i] = i;
            avgz[i] = verts[faces[i].v1].rz + verts[faces[i].v2].rz + verts[faces[i].v3].rz;
        }
        // Simple bubble sort (fine for demo sizes)
        for (int i = 0; i < nb_faces - 1; i++)
            for (int j = i + 1; j < nb_faces; j++)
                if (avgz[order[i]] < avgz[order[j]]) {
                    int t = order[i]; order[i] = order[j]; order[j] = t;
                }

        // Draw faces
        for (int i = 0; i < nb_faces; i++) {
            int fi = order[i];
            Vertex *a = &verts[faces[fi].v1];
            Vertex *b = &verts[faces[fi].v2];
            Vertex *c = &verts[faces[fi].v3];

            // Backface culling
            int cross = (b->xp - a->xp) * (c->yp - a->yp) - (c->xp - a->xp) * (b->yp - a->yp);
            if (cross >= 0) continue;

            // Shade based on face orientation (simple flat shading)
            int shade = 128 + (a->rz + b->rz + c->rz) / 8;
            if (shade < 20) shade = 20;
            if (shade > 250) shade = 250;

            fill_triangle(a->xp, a->yp, b->xp, b->yp, c->xp, c->yp, (unsigned char)shade);
        }

        // Render 8-bit buffer to 32-bit screen
        for (int i = 0; i < SW * SH; i++) {
            unsigned char c = buffer[i];
            int r = palette[c * 3] * 4; if (r > 255) r = 255;
            int g = palette[c * 3 + 1] * 4; if (g > 255) g = 255;
            int b = palette[c * 3 + 2] * 4; if (b > 255) b = 255;
            ctx->pixels[i] = RGB32(r, g, b);
        }

        demo_update(ctx);

        if (auto_rot) { angx++; angy += 2; }

        // Keyboard controls
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_M]) { motion_blur = !motion_blur; SDL_Delay(200); }
        if (keys[SDL_SCANCODE_R]) { auto_rot = !auto_rot; SDL_Delay(200); }
        if (keys[SDL_SCANCODE_Z]) ADDZ += 10;
        if (keys[SDL_SCANCODE_S]) ADDZ -= 10;
        if (!auto_rot) {
            int mx, my;
            demo_get_mouse(ctx, &mx, &my);
            angx = my;
            angy = mx;
        }
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
