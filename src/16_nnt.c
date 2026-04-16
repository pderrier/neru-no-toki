// NERU NO TOKI - "Le moment ou l'on se repose"
// (c) ZeN 1999 - Code: Wondy, Prozero, Decibel
// Full demo ported from Watcom C++ / DOS4GW / PTC / MIDAS to SDL2
//
// Parts: NNTFX1 (photo+text) -> FX2D (rotozoom+poems) -> CHAOS (landscape) -> RAYCAST
// Music: EXEL.XM then ALMA_DEL.XM

#include "demo_framework.h"

#define SW 320
#define SH 240
#define PI_F 3.14159265358979f

// ---- RGB pixel type (matches original structRGB) ----
typedef struct { unsigned char R, G, B; } RGB;

// ---- 24-bit PCX loader (3-plane, like the originals) ----
static int load_pcx_rgb(const char *path, RGB *dst, int w, int h) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", path); return -1; }
    unsigned char hdr[128];
    fread(hdr, 1, 128, f);

    int tempr = 0, tempv = 0, tempb = 0;
    int dr = 0, dv = 0, db = 0;
    int total = w * h;

    while (tempb < total) {
        // Red plane
        dr = 0;
        while (dr < w) {
            int c = fgetc(f); if (c == EOF) goto done;
            int run = 1;
            if (c > 192) { run = c - 192; c = fgetc(f); if (c == EOF) goto done; }
            for (int i = 0; i < run && tempr + i < total; i++)
                dst[tempr + i].R = (unsigned char)c;
            tempr += run; dr += run;
        }
        if (dr >= w) dr -= w;

        // Green plane
        dv = 0;
        while (dv < w) {
            int c = fgetc(f); if (c == EOF) goto done;
            int run = 1;
            if (c > 192) { run = c - 192; c = fgetc(f); if (c == EOF) goto done; }
            for (int i = 0; i < run && tempv + i < total; i++)
                dst[tempv + i].G = (unsigned char)c;
            tempv += run; dv += run;
        }
        if (dv >= w) dv -= w;

        // Blue plane
        db = 0;
        while (db < w) {
            int c = fgetc(f); if (c == EOF) goto done;
            int run = 1;
            if (c > 192) { run = c - 192; c = fgetc(f); if (c == EOF) goto done; }
            for (int i = 0; i < run && tempb + i < total; i++)
                dst[tempb + i].B = (unsigned char)c;
            tempb += run; db += run;
        }
        if (db >= w) db -= w;
    }
done:
    fclose(f);
    return 0;
}

// Same but into UCHAR array (4 bytes per pixel: R,G,B,0)
static int load_pcx_uchar(const char *path, unsigned char *dst, int w, int h) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", path); return -1; }
    unsigned char hdr[128];
    fread(hdr, 1, 128, f);

    int tempr = 0, tempv = 0, tempb = 0;
    int dr = 0, dv = 0, db = 0;
    int total = w * h;

    while (tempb < total) {
        dr = 0;
        while (dr < w) {
            int c = fgetc(f); if (c == EOF) goto done2;
            int run = 1;
            if (c > 192) { run = c - 192; c = fgetc(f); if (c == EOF) goto done2; }
            for (int i = 0; i < run && tempr + i < total; i++)
                dst[(tempr + i) * 4 + 0] = (unsigned char)c;
            tempr += run; dr += run;
        }
        if (dr >= w) dr -= w;
        dv = 0;
        while (dv < w) {
            int c = fgetc(f); if (c == EOF) goto done2;
            int run = 1;
            if (c > 192) { run = c - 192; c = fgetc(f); if (c == EOF) goto done2; }
            for (int i = 0; i < run && tempv + i < total; i++)
                dst[(tempv + i) * 4 + 1] = (unsigned char)c;
            tempv += run; dv += run;
        }
        if (dv >= w) dv -= w;
        db = 0;
        while (db < w) {
            int c = fgetc(f); if (c == EOF) goto done2;
            int run = 1;
            if (c > 192) { run = c - 192; c = fgetc(f); if (c == EOF) goto done2; }
            for (int i = 0; i < run && tempb + i < total; i++)
                dst[(tempb + i) * 4 + 2] = (unsigned char)c;
            tempb += run; db += run;
        }
        if (db >= w) db -= w;
    }
done2:
    fclose(f);
    return 0;
}

// ===========================================================================
// GLOBAL STATE
// ===========================================================================
static Uint32 *vbuffer;
static int frame_count = 0;

// Synchro: time-based scene transitions (original used MIDAS music order position)
// Each "order" in the original XM lasts ~5-6 seconds at typical BPM/speed
#define SYNC_MS 5500
static Uint32 demo_start_time;
static Uint32 part_start_time; // reset per music track
static int Synchro(void) {
    Uint32 elapsed = SDL_GetTicks() - part_start_time;
    return (int)(elapsed / SYNC_MS);
}

// ===========================================================================
// PART 1: NNTFX1 - Photos with directional blur text overlay
// ===========================================================================
static RGB *nntfx_img;          // 320*384 text strips
static unsigned char *photo[5]; // 5 photos (UCHAR, 4 bytes/pixel)
static RGB *tmpbuf;             // temp buffer

static int nntfx1_init(void) {
    nntfx_img = (RGB*)calloc(320 * 384, sizeof(RGB));
    tmpbuf = (RGB*)calloc(SW * SH, sizeof(RGB));
    if (!nntfx_img || !tmpbuf) return -1;

    const char *photo_files[] = {
        "data/nnt/NNT_BRUI.PCX", "data/nnt/NNT_POLL.PCX",
        "data/nnt/NNT_HAIN.PCX", "data/nnt/NNT_STRE.PCX", "data/nnt/NNT_CONN.PCX"
    };
    for (int i = 0; i < 5; i++) {
        photo[i] = (unsigned char*)calloc(4 * SW * SH, 1);
        if (!photo[i]) return -1;
        load_pcx_uchar(photo_files[i], photo[i], SW, SH);
    }
    load_pcx_rgb("data/nnt/NNT_TEXT.PCX", nntfx_img, 320, 384);

    // Convert text to grayscale (max of R,G,B -> R)
    for (int i = 0; i < 320 * 384; i++) {
        if (nntfx_img[i].R < nntfx_img[i].G) nntfx_img[i].R = nntfx_img[i].G;
        if (nntfx_img[i].R < nntfx_img[i].B) nntfx_img[i].R = nntfx_img[i].B;
    }
    return 0;
}

static void nntfx1_render(int syncc) {
    int photoIdx = syncc;
    if (photoIdx < 0) photoIdx = 0;
    if (photoIdx > 4) photoIdx = 4;

    int iDecalageTexte = 320 * 32 + (320 * 64) * photoIdx;
    int randfondX = rand() % 11;
    int randfondY = rand() % 11;

    // Directional blur on text
    int iNbPixFlou = rand() % 10;
    int iDiviseur = iNbPixFlou * 2 + 1;
    if (iDiviseur < 1) iDiviseur = 1;

    memset(tmpbuf, 0, sizeof(RGB) * SW * SH);

    for (int x = 0; x < 320; x++) {
        int col = 0;
        for (int t = -iNbPixFlou; t <= iNbPixFlou; t++) {
            int idx = x + t * 320 + iDecalageTexte;
            if (idx >= 0 && idx < 320 * 384) col += nntfx_img[idx].R;
        }
        for (int y = 0; y < 64; y++) {
            int dstIdx = x + (120 - 32 + y) * 320;
            if (dstIdx >= 0 && dstIdx < SW * SH)
                tmpbuf[dstIdx].R = (unsigned char)(col / iDiviseur);
            int addIdx = x + (y + iNbPixFlou) * 320 + iDecalageTexte;
            int subIdx = x + (y - iNbPixFlou) * 320 + iDecalageTexte;
            if (addIdx >= 0 && addIdx < 320 * 384) col += nntfx_img[addIdx].R;
            if (subIdx >= 0 && subIdx < 320 * 384) col -= nntfx_img[subIdx].R;
        }
    }

    // Composite photo + text
    for (int y = 0; y < SH - 10; y++) {
        for (int x = 0; x < SW - 10; x++) {
            int ofs = x + y * SW;
            int srcOfs = (ofs + randfondX + randfondY * SW) * 4;
            if (srcOfs + 2 >= 4 * SW * SH) continue;
            int R = photo[photoIdx][srcOfs];
            int G = photo[photoIdx][srcOfs + 1];
            int B = photo[photoIdx][srcOfs + 2];
            if (tmpbuf[ofs].R > 0) {
                R += tmpbuf[ofs].R; G += tmpbuf[ofs].R; B += tmpbuf[ofs].R;
                if (R > 255) R = 255; if (G > 255) G = 255; if (B > 255) B = 255;
            }
            vbuffer[ofs] = RGB32(R, G, B);
        }
    }
}

static void nntfx1_free(void) {
    free(nntfx_img);
    for (int i = 0; i < 5; i++) free(photo[i]);
}

// ===========================================================================
// PART 2: FX2D - Rotozoom with poems overlay
// ===========================================================================
static RGB *fx2d_tex;
static RGB *poemes[3];
static unsigned char fx2d_prof[640 * 480];
static unsigned fx2d_tatan2[480 * 640];
static int fx2d_cos1[256], fx2d_sin1[256], fx2d_cos2[256], fx2d_sin2[256];
static int fx2d_cos3[256], fx2d_sin3[256], fx2d_sin4[256];

static int fx2d_init(void) {
    fx2d_tex = (RGB*)calloc(256 * 256, sizeof(RGB));
    if (!fx2d_tex) return -1;
    load_pcx_rgb("data/nnt/FX2D.PCX", fx2d_tex, 256, 256);
    for (int i = 0; i < 3; i++) {
        poemes[i] = (RGB*)calloc(SW * SH, sizeof(RGB));
        if (!poemes[i]) return -1;
    }
    load_pcx_rgb("data/nnt/POEME.PCX", poemes[0], 320, 240);
    load_pcx_rgb("data/nnt/POEME2.PCX", poemes[1], 320, 240);
    load_pcx_rgb("data/nnt/POEME3.PCX", poemes[2], 320, 240);

    for (int y = -240; y < 240; y++)
        for (int x = -320; x < 320; x++) {
            double a = atan2((double)y, (double)x);
            fx2d_tatan2[(y + 240) * 640 + x + 320] = (unsigned)(a * 256.0 / PI_F);
            fx2d_prof[(y + 240) * 640 + x + 320] = (unsigned char)sqrt((double)x * x + (double)y * y);
        }
    for (int x = 0; x < 256; x++) {
        fx2d_cos1[x] = (int)(10 * cos((x << 1) * PI_F / 128) + 80);
        fx2d_sin1[x] = (int)(50 * sin((x << 1) * PI_F / 128) + 75);
        fx2d_cos2[x] = (int)(70 * cos(x * PI_F / 128) + 80);
        fx2d_sin2[x] = (int)(30 * sin((x + 10) * PI_F / 128) + 175);
        fx2d_cos3[x] = (int)(70 * cos((x + 220) * PI_F / 128) + 140);
        fx2d_sin3[x] = (int)(2 * sin((x + 50) * PI_F / 128) + 190);
        fx2d_sin4[x] = (int)(sin(x * PI_F / 128) + 17.0);
    }
    return 0;
}

static unsigned fx2d_xaddi = 0, fx2d_yaddi = 0;
static unsigned char fx2d_ccx = 0, fx2d_ccy = 0;
static int fx2d_decaly = 0, fx2d_decalx = 0;

static void fx2d_render(int numpoeme) {
    if (numpoeme < 0) numpoeme = 0;
    if (numpoeme > 2) numpoeme = 2;

    fx2d_ccy += 3; fx2d_ccx += 2;
    int dyang1 = fx2d_decaly, dxang1 = fx2d_decalx;
    int dyang2 = fx2d_cos1[fx2d_ccy], dxang2 = fx2d_sin1[fx2d_ccx];
    int dyang3 = fx2d_cos2[fx2d_ccy], dxang3 = fx2d_sin2[fx2d_ccy];
    fx2d_decaly = fx2d_cos3[fx2d_ccy];
    fx2d_decalx = fx2d_sin3[fx2d_ccx];

    int totdeca = fx2d_decaly * 640 + fx2d_decalx;

    for (int y = 0; y < SH; y++) {
        unsigned off1 = (y + dyang1) * 640 + dxang1;
        unsigned off2 = (y + dyang2) * 640 + dxang2;
        unsigned off3 = (y + dyang3) * 640 + dxang3;
        for (int x = 0; x < SW; x++) {
            int ofs = y * SW + x;
            if (off1 + x >= 640 * 480 || off2 + x >= 640 * 480 || off3 + x >= 640 * 480) continue;

            unsigned char ty = fx2d_prof[y * 640 + x + totdeca < 640 * 480 ? y * 640 + x + totdeca : 0];
            int txtofs = ((ty << 8) + fx2d_tatan2[off1 + x] - fx2d_tatan2[off2 + x] + fx2d_tatan2[off3 + x] + fx2d_xaddi) & 0xFFFF;
            if (txtofs >= 256 * 256) txtofs %= 256 * 256;

            // Motion blur: average with previous frame
            Uint32 old = vbuffer[ofs];
            int R = (fx2d_tex[txtofs].R + ((old >> 16) & 255)) >> 1;
            int G = (fx2d_tex[txtofs].G + ((old >> 8) & 255)) >> 1;
            int B = (fx2d_tex[txtofs].B + (old & 255)) >> 1;

            // Subtract poem text
            int zob = ofs + ((fx2d_decaly - 140) >> 4);
            if (zob >= 0 && zob < SW * SH && poemes[numpoeme][zob].R) {
                R -= poemes[numpoeme][zob].R;
                G -= poemes[numpoeme][zob].G;
                B -= poemes[numpoeme][zob].B;
                if (R < 0) R = 0; if (G < 0) G = 0; if (B < 0) B = 0;
            }
            vbuffer[ofs] = RGB32(R, G, B);
        }
    }
    fx2d_yaddi += fx2d_sin4[fx2d_ccx];
    fx2d_xaddi += 3;
}

static void fx2d_free(void) {
    free(fx2d_tex);
    for (int i = 0; i < 3; i++) free(poemes[i]);
}

// ===========================================================================
// PART 3: KATA - Yoga guy + water reflection (original by Wondy)
// ===========================================================================
static RGB *kata_fond;   // 320*240 background (zen garden)
static RGB *kata_anim;   // 224*352 animation strip (bonhomme yoga)
static double kata_sinkata[360];
static double kata_sinkata2[64];
static int kata_vaguec = 0;
static unsigned kata_ofsanim = 0;
static unsigned kata_c = 0;
static unsigned kata_meuh = 0;

static int kata_init(void) {
    kata_fond = (RGB*)calloc(SW * SH, sizeof(RGB));
    kata_anim = (RGB*)calloc(224 * 352, sizeof(RGB));
    if (!kata_fond || !kata_anim) return -1;
    load_pcx_rgb("data/nnt/KAT_FOND.PCX", kata_fond, 320, 240);
    load_pcx_rgb("data/nnt/KAT_ANIM.PCX", kata_anim, 224, 352);

    for (int i = 0; i < 360; i++) kata_sinkata[i] = sin((double)i * PI_F / 180.0) * 2.0;
    for (int i = 0; i < 64; i++) kata_sinkata2[i] = sin((double)i * 20.0 / PI_F) * 1.8;
    return 0;
}

static void kata_render(void) {
    // Draw background (top half only, bottom = water)
    for (int i = 0; i < SW * 120; i++)
        vbuffer[i] = RGB32(kata_fond[i].R, kata_fond[i].G, kata_fond[i].B);

    // Draw animated character at (156, 42), sprite is 32x32 inside 224-wide strip
    int offset = 0;
    for (int y = 42; y < 42 + 32; y++) {
        for (int x = 156; x < 156 + 32; x++) {
            int src = kata_ofsanim + offset;
            if (src >= 0 && src < 224 * 352 && x < SW && y < SH) {
                RGB px = kata_anim[src];
                vbuffer[x + y * SW] = RGB32(px.R, px.G, px.B);
            }
            offset++;
        }
        offset += 192; // skip to next row in 224-wide strip (224 - 32 = 192)
    }

    // Water reflection effect (y >= 120): mirror top half with sine distortion + blue tint
    int offset2 = 120 * SW;
    for (int y = 120; y < SH; y++) {
        int val = (int)kata_sinkata2[(y + kata_vaguec) & 63];
        int mirror_y = (239 - y + val);
        if (mirror_y < 0) mirror_y = 0;
        if (mirror_y >= SH) mirror_y = SH - 1;
        int srcrow = mirror_y * SW;
        for (int x = 0; x < SW; x++) {
            int wave_x = x + (int)kata_sinkata2[(x + kata_vaguec) & 63];
            if (wave_x < 0) wave_x = 0;
            if (wave_x >= SW) wave_x = SW - 1;
            Uint32 src_pixel = vbuffer[srcrow + wave_x];
            // Darken + blue tint (original: R>>3, G>>2, B as-is)
            int R = ((src_pixel >> 16) & 255) >> 3;
            int G = ((src_pixel >> 8) & 255) >> 2;
            int B = (src_pixel & 255);
            vbuffer[offset2] = RGB32(R, G, B);
            offset2++;
        }
    }

    // Advance animation (original timing: every 5 frames, 7 frames per row)
    if (++kata_meuh >= 5) {
        kata_meuh = 0;
        if (kata_c >= 6) {
            kata_c = 0;
            kata_ofsanim += 31 * 224 + 32; // next row
        } else {
            kata_ofsanim += 32; // next frame in row
            kata_c++;
        }
        if (kata_ofsanim >= (unsigned)(224 * 352)) { kata_ofsanim = 0; kata_c = 0; }
    }
    kata_vaguec = (kata_vaguec + 3) & 63;
}

static void kata_free(void) {
    free(kata_fond); free(kata_anim);
}

// ===========================================================================
// PART 4: CHAOS - Parallax landscape with bump fog and scrolling text
// ===========================================================================
static RGB *chaos_fond;    // 3064*200 landscape
static RGB *chaos_bump;    // 960*200 fog
static RGB *chaos_img2;    // 320*705 text
static RGB *chaos_tmpbuf;

static int chaos_init(void) {
    chaos_fond = (RGB*)calloc(3064 * 200, sizeof(RGB));
    chaos_bump = (RGB*)calloc(960 * 200, sizeof(RGB));
    chaos_img2 = (RGB*)calloc(320 * 705, sizeof(RGB));
    chaos_tmpbuf = (RGB*)calloc(SW * SH, sizeof(RGB));
    if (!chaos_fond || !chaos_bump || !chaos_img2 || !chaos_tmpbuf) return -1;

    load_pcx_rgb("data/nnt/CH_PAYSA.PCX", chaos_fond, 3064, 200);
    load_pcx_rgb("data/nnt/CH_DECAL.PCX", chaos_bump, 960, 200);
    load_pcx_rgb("data/nnt/CH_TEXTE.PCX", chaos_img2, 320, 705);

    // Convert text to grayscale
    for (int i = 0; i < 320 * 705; i++) {
        if (chaos_img2[i].R < chaos_img2[i].G) chaos_img2[i].R = chaos_img2[i].G;
        if (chaos_img2[i].R < chaos_img2[i].B) chaos_img2[i].R = chaos_img2[i].B;
    }
    return 0;
}

static unsigned chaos_posx = 0, chaos_posx2 = 0, chaos_posx3 = 0;
static int chaos_sin[256];
static unsigned chaos_compteur = 0;
static int chaos_Tpos;
static int chaos_iDecalageTexte = 0;

static void chaos_render(int syncc) {
    // Blur on text
    int nbflou = chaos_sin[chaos_compteur & 255];
    chaos_compteur++;
    int diviseur = nbflou * 2 + 1;
    if (diviseur < 1) diviseur = 1;

    chaos_iDecalageTexte = 320 * 64 * (syncc - 4);
    if (chaos_iDecalageTexte < 0) chaos_iDecalageTexte = 0;

    memset(chaos_tmpbuf, 0, SW * SH * sizeof(RGB));
    chaos_Tpos = 20 * 320;

    for (int x = 0; x < 320; x++) {
        int col = 0;
        for (int j = -nbflou; j <= nbflou; j++) {
            int idx = x + j * 320 + chaos_iDecalageTexte;
            if (idx >= 0 && idx < 320 * 705) col += chaos_img2[idx].R;
        }
        for (int y = 0; y < 64; y++) {
            int dst = x + chaos_Tpos + y * 320;
            if (dst >= 0 && dst < SW * SH)
                chaos_tmpbuf[dst].R = (unsigned char)(col / diviseur);
            int ai = x + (y + nbflou) * 320 + chaos_iDecalageTexte;
            int si = x + (y - nbflou) * 320 + chaos_iDecalageTexte;
            if (ai >= 0 && ai < 320 * 705) col += chaos_img2[ai].R;
            if (si >= 0 && si < 320 * 705) col -= chaos_img2[si].R;
        }
    }

    // Composite: landscape + fog + text
    unsigned offset = 20 * 320;
    unsigned offset2 = chaos_posx;
    unsigned offset3 = chaos_posx2;
    for (int i = 20; i < 220; i++) {
        for (int j = 0; j < 320; j++) {
            if (offset3 < 960 * 200 && (960 * 200 - offset3 - chaos_posx3 + chaos_posx2) < 960 * 200) {
                int C = (chaos_bump[offset3].R + chaos_bump[960 * 200 - 1 - offset3].R) >> 1;
                int C2 = offset2 + (C >> 4) - 8;
                if (C2 < 0) C2 = 0;
                if (C2 >= 3064 * 200) C2 = 3064 * 200 - 1;
                int textval = (offset < (unsigned)(SW * SH)) ? chaos_tmpbuf[offset].R : 0;
                int R = textval + ((C + chaos_fond[C2].R) >> 1);
                int G = textval + ((C + chaos_fond[C2].G) >> 1);
                int B = textval + ((C + chaos_fond[C2].B) >> 1);
                if (R > 255) R = 255; if (G > 255) G = 255; if (B > 255) B = 255;
                if (offset < (unsigned)(SW * SH))
                    vbuffer[offset] = RGB32(R, G, B);
            }
            offset++; offset2++; offset3++;
        }
        offset2 += 2744; offset3 += 640;
    }
    chaos_posx += 4; if (chaos_posx > 2744) chaos_posx = 0;
    chaos_posx2 += 6; if (chaos_posx2 > 640) chaos_posx2 = 0;
    chaos_posx3 += 8; if (chaos_posx3 > 640) chaos_posx3 = 0;
}

static void chaos_free(void) {
    free(chaos_fond); free(chaos_bump); free(chaos_img2); free(chaos_tmpbuf);
}

// ===========================================================================
// PART 4: RAYCAST - Raytraced tunnel/plane/spheres with transitions
// ===========================================================================
typedef struct { float x, y, z; } vec3;
typedef struct { int x, y, l; } ivec3;

static char *ray_map1;
static unsigned char *ray_texture;
static RGB *ray_fond;
static vec3 ray_precaldir[40 * 25];
static ivec3 ray_tmpbuf[40 * 25];
static int ray_effect1 = 2, ray_effect2 = 2;
static float ray_transition = 0.0f;
static float ray_sphere_x[4], ray_sphere_y[4];

static int raycast_init(void) {
    RGB *tmp = (RGB*)calloc(512 * 512, sizeof(RGB));
    ray_map1 = (char*)calloc(512 * 512, 1);
    ray_texture = (unsigned char*)calloc(256 * 256 * 4, 1);
    ray_fond = (RGB*)calloc(320 * 200, sizeof(RGB));
    if (!tmp || !ray_map1 || !ray_texture || !ray_fond) return -1;

    load_pcx_rgb("data/nnt/RAY_MAP1.PCX", tmp, 512, 512);
    for (int i = 0; i < 512 * 512; i++) ray_map1[i] = tmp[i].R;
    free(tmp);

    load_pcx_uchar("data/nnt/RAY_TEXT.PCX", ray_texture, 256, 256);
    load_pcx_rgb("data/nnt/RAY_FOND.PCX", ray_fond, 320, 200);

    // Precalculate ray directions
    for (int y = 0; y < 25; y++)
        for (int x = 0; x < 40; x++) {
            vec3 d = { (float)(x * 8 - 160), (float)(y * 8 - 100), 128.0f };
            float len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
            d.x /= len; d.y /= len; d.z /= len;
            ray_precaldir[x + y * 40] = d;
        }
    return 0;
}

static void rotate_vec(vec3 *r, float ax, float ay, float az) {
    float my = r->y * cosf(ax) - r->z * sinf(ax);
    float mz = r->y * sinf(ax) + r->z * cosf(ax);
    r->y = my; r->z = mz;
    float mx = r->x * cosf(ay) + r->z * sinf(ay);
    mz = -r->x * sinf(ay) + r->z * cosf(ay);
    r->x = mx; r->z = mz;
    mx = r->x * cosf(az) - r->y * sinf(az);
    my = r->x * sinf(az) + r->y * cosf(az);
    r->x = mx; r->y = my;
}

static ivec3 raycast_tunnel(int x, int y, float angx, float angy, float angz, int tudecaz, float noise) {
    vec3 d = ray_precaldir[x + y * 40];
    rotate_vec(&d, angx, angy, angz * 2);
    float a = d.x * d.x + d.y * d.y;
    float b = 2 * (-128 * d.z); // o.z=-128
    (void)b;
    float rayon = sinf(5 * atan2f(d.x, d.y)) * noise + 256;
    float c = 128.0f * 128.0f - rayon * rayon; // simplified
    float delta = 4 * a * (-c); // simplified
    if (delta < 0) delta = 0;
    float t = sqrtf(delta) / (2 * a + 0.001f);
    float iz = -128.0f + t * d.z;
    ivec3 fin;
    fin.x = (int)(iz * 65536.0f / 3.0f) + tudecaz * 65536;
    fin.y = (int)fabsf(atan2f(t * d.y, t * d.x) * 256 * 65536 / PI_F);
    fin.l = (int)((iz > 1024 ? 1024 : iz) * 65536.0f) / 10;
    return fin;
}

static ivec3 raycast_plan(int x, int y, float angx, float angy, float angz, float pldecaz, int tudecaz, float decasin, int dmx, int dmy) {
    vec3 d = ray_precaldir[x + y * 40];
    rotate_vec(&d, angx, angy, angz);
    float t = (400 + pldecaz) / (d.z + 0.001f);
    float ix = t * d.x, iy = t * d.y;
    int mapx = (((int)(ix / 6) + dmx) & 511);
    int mapy = (((int)(iy / 6) + dmy) & 511);
    float iz = sinf(ray_map1[mapx + mapy * 512] * PI_F / 128 + decasin) * 30;
    float doi = sqrtf(ix * ix + iy * iy);
    float tmpz = doi < 1555 ? 0 : (doi > 3580 ? 356 : (doi - 1555) * 356 / (3580 - 1555));
    if (t < 0) iz = -1000;
    ivec3 fin;
    fin.x = (int)((ix * 256 / (iz + 312 + 0.001f) + 128) * 65536) + tudecaz * 65536;
    fin.y = (int)((iy * 256 / (iz + 312 + 0.001f) + 128) * 65536);
    fin.l = (int)((tmpz - iz) * 65536);
    return fin;
}

static void raycast_render(int frames_local) {
    float plangx = sinf(frames_local * PI_F / 110) / 2.5f;
    float plangy = cosf(frames_local * PI_F / 70) / 3.0f;
    float plangz = (float)frames_local / 90.0f;
    float pldecaz = sinf(frames_local * PI_F / 100) * 200;
    float tuangy = (float)frames_local / 40.0f;
    int tudecaz = frames_local * 3;
    float noise = sinf(frames_local * PI_F / 50) * 130;
    float decasin = frames_local * 0.2f;
    int dmx = frames_local * 3, dmy = frames_local;

    // Low-res raycast (40x25 blocks, interpolated to 320x200)
    for (int y = 0; y < 25; y++)
        for (int x = 0; x < 40; x++) {
            ivec3 f1, f2, fin;
            if (ray_effect1 == ray_effect2) {
                switch (ray_effect1) {
                    case 1: fin = raycast_tunnel(x, y, plangx, tuangy, plangz, tudecaz, noise); break;
                    case 2: fin = raycast_plan(x, y, plangx, plangy, plangz, pldecaz, tudecaz, decasin, dmx, dmy); break;
                    default: fin = raycast_plan(x, y, plangx, plangy, plangz, pldecaz, tudecaz, decasin, dmx, dmy); break;
                }
            } else {
                switch (ray_effect1) {
                    case 1: f1 = raycast_tunnel(x, y, plangx, tuangy, plangz, tudecaz, noise); break;
                    default: f1 = raycast_plan(x, y, plangx, plangy, plangz, pldecaz, tudecaz, decasin, dmx, dmy); break;
                }
                switch (ray_effect2) {
                    case 1: f2 = raycast_tunnel(x, y, plangx, tuangy, plangz, tudecaz, noise); break;
                    default: f2 = raycast_plan(x, y, plangx, plangy, plangz, pldecaz, tudecaz, decasin, dmx, dmy); break;
                }
                fin.x = (int)((f2.x - f1.x) * ray_transition + f1.x);
                fin.y = (int)((f2.y - f1.y) * ray_transition + f1.y);
                fin.l = (int)((f2.l - f1.l) * ray_transition + f1.l);
            }
            ray_tmpbuf[x + y * 40] = fin;
        }

    // Bilinear interpolation to fill 8x8 blocks
    for (int by = 0; by < 24; by++)
        for (int bx = 0; bx < 39; bx++) {
            ivec3 tl = ray_tmpbuf[bx + by * 40];
            ivec3 tr = ray_tmpbuf[bx + 1 + by * 40];
            ivec3 bl = ray_tmpbuf[bx + (by + 1) * 40];
            ivec3 br = ray_tmpbuf[bx + 1 + (by + 1) * 40];

            for (int iy = 0; iy < 8; iy++) {
                float fy = iy / 8.0f;
                for (int ix = 0; ix < 8; ix++) {
                    float fx = ix / 8.0f;
                    int px = bx * 8 + ix + 2;
                    int py = by * 8 + iy + 2 + 20; // +20 offset like original
                    if (px >= SW || py >= SH) continue;

                    float xf = tl.x * (1 - fx) * (1 - fy) + tr.x * fx * (1 - fy) + bl.x * (1 - fx) * fy + br.x * fx * fy;
                    float yf = tl.y * (1 - fx) * (1 - fy) + tr.y * fx * (1 - fy) + bl.y * (1 - fx) * fy + br.y * fx * fy;
                    float lf = tl.l * (1 - fx) * (1 - fy) + tr.l * fx * (1 - fy) + bl.l * (1 - fx) * fy + br.l * fx * fy;

                    int ofstex = (((int)(xf) >> 16) & 255) + ((((int)(yf) >> 8)) & (65280));
                    if (ofstex < 0) ofstex = 0;
                    if (ofstex >= 256 * 256) ofstex %= 256 * 256;

                    int fondofs = px + (py - 20) * 320;
                    if (fondofs >= 0 && fondofs < 320 * 200 &&
                        (ray_fond[fondofs].R || ray_fond[fondofs].G || ray_fond[fondofs].B)) {
                        vbuffer[py * SW + px] = RGB32(ray_fond[fondofs].R, ray_fond[fondofs].G, ray_fond[fondofs].B);
                    } else {
                        int R = ray_texture[ofstex * 4 + 0];
                        int G = ray_texture[ofstex * 4 + 1];
                        int B = ray_texture[ofstex * 4 + 2];
                        int dim = (int)(lf) >> 15;
                        R -= dim; G -= dim; B -= dim;
                        if (R < 0) R = 0; if (G < 0) G = 0; if (B < 0) B = 0;
                        if (R > 255) R = 255; if (G > 255) G = 255; if (B > 255) B = 255;
                        vbuffer[py * SW + px] = RGB32(R, G, B);
                    }
                }
            }
        }

    ray_transition += 0.02f;
    if (ray_transition > 1.0f) { ray_transition = 0.0f; ray_effect1 = ray_effect2; }
}

static void raycast_free(void) {
    free(ray_map1); free(ray_texture); free(ray_fond);
}

// ===========================================================================
// MAIN - Full demo sequence
// ===========================================================================
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    DemoContext *ctx = demo_init("NERU NO TOKI - ZeN 1999", SW, SH);
    if (!ctx) return 1;
    vbuffer = ctx->pixels;

    // Load music
    Mix_Music *music1 = demo_load_music("data/nnt/EXEL.XM");
    demo_play_music(music1);

    // Init sin table for chaos
    for (int i = 0; i < 256; i++)
        chaos_sin[i] = (int)(sinf(i * PI_F / 4) * 3.9f) + 4;

    // Init all parts
    if (nntfx1_init() < 0) { fprintf(stderr, "NNTFX1 init failed\n"); goto cleanup; }
    if (fx2d_init() < 0) { fprintf(stderr, "FX2D init failed\n"); goto cleanup; }
    if (kata_init() < 0) { fprintf(stderr, "KATA init failed\n"); goto cleanup; }
    if (chaos_init() < 0) { fprintf(stderr, "CHAOS init failed\n"); goto cleanup; }
    if (raycast_init() < 0) { fprintf(stderr, "RAYCAST init failed\n"); goto cleanup; }

    printf("NERU NO TOKI loaded. Starting demo...\n");
    demo_start_time = SDL_GetTicks();
    part_start_time = demo_start_time;
    int ray_frames = 0;
    int switched_music = 0;
    int current_part = 0; // 0=NNTFX1, 1=RADIAL, 2=FX2D, 3=KATA, 4=CHAOS, 5=RAYCAST, 6=END

    while (demo_poll(ctx)) {
        int syncc = Synchro();

        switch (current_part) {
        // ---- NNTFX1: photos + blurred text (orders 0-4 on EXEL.XM) ----
        case 0:
            nntfx1_render(syncc);
            if (syncc >= 5) {
                current_part = 1;
                frame_count = 0;
            }
            break;

        // ---- RADIAL: volume fade transition (128 frames) ----
        case 1: {
            int v = 128 - frame_count;
            if (v < 1) v = 1;
            demo_set_volume(v / 2);
            // Keep displaying last frame with slight movement
            float xo = 160.0f + sinf(frame_count * PI_F / 50) * 20 + cosf(frame_count * PI_F / 40) * 40;
            float yo = 130.0f + sinf(frame_count * PI_F / 60) * 15;
            (void)xo; (void)yo;
            // Fade to black
            for (int i = 0; i < SW * SH; i++) {
                Uint32 p = vbuffer[i];
                int R = ((p >> 16) & 255); R -= 2; if (R < 0) R = 0;
                int G = ((p >> 8) & 255);  G -= 2; if (G < 0) G = 0;
                int B = (p & 255);         B -= 2; if (B < 0) B = 0;
                vbuffer[i] = RGB32(R, G, B);
            }
            if (frame_count >= 128) {
                // Switch music
                demo_stop_music();
                demo_free_music(music1);
                music1 = demo_load_music("data/nnt/ALMA_DEL.XM");
                demo_play_music_once(music1); // play once, demo ends with music
                demo_set_volume(64);
                switched_music = 1;
                memset(vbuffer, 0, SW * SH * sizeof(Uint32));
                current_part = 2;
                part_start_time = SDL_GetTicks(); // reset synchro for ALMA_DEL
                frame_count = 0;
            }
            break;
        }

        // ---- FX2D: rotozoom + poems (orders 0-2 on ALMA_DEL.XM) ----
        case 2:
            fx2d_render(syncc);
            if (syncc >= 3) {
                current_part = 3;
                frame_count = 0;
            }
            break;

        // ---- KATA: yoga bonhomme + water reflection (orders 3-4) ----
        case 3:
            kata_render();
            if (syncc >= 5) {
                memset(vbuffer, 0, SW * SH * sizeof(Uint32));
                current_part = 4;
                frame_count = 0;
            }
            break;

        // ---- CHAOS: parallax landscape + credits (orders 5-12) ----
        case 4:
            chaos_render(syncc);
            if (syncc >= 13) {
                memset(vbuffer, 0, SW * SH * sizeof(Uint32));
                current_part = 5;
                frame_count = 0;
                ray_frames = 0;
            }
            break;

        // ---- RAYCAST: tunnel/plane/spheres (until music ends) ----
        case 5:
            if (ray_frames > 60) { ray_effect2 = 3; } // transition to spheres
            if (ray_frames > 200) { ray_effect2 = 1; } // transition to tunnel
            raycast_render(ray_frames);
            ray_frames++;
            // End when music stops or time exceeded
            if ((switched_music && !demo_music_playing()) || syncc >= 0x20) {
                current_part = 6;
                frame_count = 0;
            }
            break;

        // ---- END: fade out ----
        case 6:
            for (int i = 0; i < SW * SH; i++) {
                Uint32 p = vbuffer[i];
                int R = ((p >> 16) & 255); R -= 4; if (R < 0) R = 0;
                int G = ((p >> 8) & 255);  G -= 4; if (G < 0) G = 0;
                int B = (p & 255);         B -= 4; if (B < 0) B = 0;
                vbuffer[i] = RGB32(R, G, B);
            }
            if (frame_count > 200) ctx->running = 0; // quit after fade
            break;
        }

        frame_count++;
        demo_update(ctx);
    }

cleanup:
    nntfx1_free();
    fx2d_free();
    kata_free();
    chaos_free();
    raycast_free();
    free(tmpbuf);

    printf("\n\t                     NERU NO TOKI\n");
    printf("\t            \"Le moment ou l'on se repose\"\n");
    printf("\t                        ZeN 1999\n\n");

    demo_stop_music();
    demo_free_music(music1);
    demo_close(ctx);
    return 0;
}
