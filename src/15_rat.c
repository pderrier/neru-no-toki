// RAT - Original by WondY / ZeN
// Autonomous agent navigating a maze
// Ported from Watcom C Mode 13h to SDL2

#include "demo_framework.h"

#define MW 320
#define MH 200

#define DROITE  1
#define GAUCHE -1
#define HAUT   -1
#define BAS     1

typedef struct { int x, y; } Mobile;

short maze[322 * 202];     // maze layout
short maze_orig[322 * 202]; // original copy
unsigned int visit_count[MW * MH];
unsigned char display[MW * MH];

Mobile rat;
int deplx = 1, deply = 0;

void generate_maze(void) {
    // Generate a procedural maze
    memset(maze, 0, sizeof(maze));

    // Walls
    for (int y = 0; y < 202; y++)
        for (int x = 0; x < 322; x++) {
            int idx = y * 320 + x;
            if (idx < 0 || idx >= 322 * 202) continue;
            maze[idx] = 1; // default: walkable
        }

    // Border walls
    for (int x = 0; x < 322; x++) {
        maze[x] = -1;
        maze[201 * 320 + x] = -1;
    }
    for (int y = 0; y < 202; y++) {
        maze[y * 320] = -1;
        maze[y * 320 + 321] = -1;
    }

    // Internal walls (create a simple maze pattern)
    for (int y = 20; y < 180; y += 40)
        for (int x = 20; x < 300; x++)
            if ((x % 80) < 60)
                maze[(y + 321) ] = -1, maze[y * 320 + x + 321] = -1;

    for (int x = 40; x < 280; x += 60)
        for (int y = 40; y < 180; y++)
            if ((y % 60) < 40)
                maze[y * 320 + x + 321] = -1;

    // Some random obstacles
    srand(42);
    for (int i = 0; i < 200; i++) {
        int x = rand() % 300 + 10;
        int y = rand() % 180 + 10;
        for (int dy = 0; dy < 5; dy++)
            for (int dx = 0; dx < 5; dx++)
                if ((y + dy) * 320 + x + dx + 321 < 322 * 202)
                    maze[(y + dy) * 320 + x + dx + 321] = -1;
    }

    memcpy(maze_orig, maze, sizeof(maze));
}

void calc_movement(void) {
    int td = 0, tg = 0, th = 0, tb = 0;

    // Look in each direction for walls
    for (int x = rat.x + 1; x <= 320; x++) {
        int idx = x + rat.y * 320 + 321;
        if (idx >= 0 && idx < 322 * 202) {
            if (maze[idx] == 1 || td <= 0) td += maze[idx];
            else break;
        } else break;
    }
    for (int x = rat.x - 1; x >= -1; x--) {
        int idx = x + rat.y * 320 + 321;
        if (idx >= 0 && idx < 322 * 202) {
            if (maze[idx] == 1 || tg <= 0) tg += maze[idx];
            else break;
        } else break;
    }
    for (int y = rat.y + 1; y <= 201; y++) {
        int idx = y * 320 + rat.x + 321;
        if (idx >= 0 && idx < 322 * 202) {
            if (maze[idx] == 1 || tb <= 0) tb += maze[idx];
            else break;
        } else break;
    }
    for (int y = rat.y - 1; y >= -1; y--) {
        int idx = y * 320 + rat.x + 321;
        if (idx >= 0 && idx < 322 * 202) {
            if (maze[idx] == 1 || th <= 0) th += maze[idx];
            else break;
        } else break;
    }

    // Scale vertical to match aspect ratio
    tb = (int)((float)tb * 320.0f / 200.0f);
    th = (int)((float)th * 320.0f / 200.0f);

    if (td > tg) deplx = DROITE;
    if (td < tg) deplx = GAUCHE;
    if (th > tb) deply = HAUT;
    if (th < tb) deply = BAS;
    if (td == tg) deplx = (rand() % 100 < 50) ? DROITE : GAUCHE;
    if (th == tb) deply = (rand() % 100 < 50) ? BAS : HAUT;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    DemoContext *ctx = demo_init("RAT - WondY / ZeN", MW, MH);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/ZINZIN.XM");
    demo_play_music(music);

    generate_maze();
    memset(visit_count, 0, sizeof(visit_count));
    rat.x = 10; rat.y = 75;

    while (demo_poll(ctx)) {
        // Render maze
        for (int i = 0; i < MW * MH; i++) {
            int idx = i + 321;
            if (idx >= 0 && idx < 322 * 202) {
                if (maze_orig[idx] == -1)
                    display[i] = 128; // wall
                else
                    display[i] = 0;   // floor
            }
        }

        // Draw rat trail (heat map)
        for (int y = 0; y < MH; y++) {
            for (int x = 0; x < MW; x++) {
                unsigned int v = visit_count[x + y * MW];
                if (v > 0) {
                    int c = (int)(v * 4);
                    if (c > 60) c = 60;
                    display[y * MW + x] = (unsigned char)(30 + c);
                }
            }
        }

        // Draw rat
        if (rat.x >= 0 && rat.x < MW - 1 && rat.y >= 0 && rat.y < MH - 1) {
            display[rat.y * MW + rat.x] = 255;
            display[rat.y * MW + rat.x + 1] = 255;
            display[(rat.y + 1) * MW + rat.x] = 255;
            display[(rat.y + 1) * MW + rat.x + 1] = 255;
        }

        // Render to screen
        for (int i = 0; i < MW * MH; i++) {
            unsigned char c = display[i];
            if (c == 128)
                ctx->pixels[i] = RGB32(80, 80, 120); // wall
            else if (c == 255)
                ctx->pixels[i] = RGB32(255, 255, 0); // rat
            else if (c > 30)
                ctx->pixels[i] = RGB32(c * 3, c, c / 2); // trail
            else
                ctx->pixels[i] = RGB32(20, 20, 30); // floor
        }

        demo_update(ctx);

        // Update rat (multiple steps per frame for speed)
        for (int step = 0; step < 3; step++) {
            calc_movement();
            rat.x += deplx;
            rat.y += deply;
            if (rat.x <= 0) rat.x = 1;
            if (rat.x >= MW) rat.x = MW - 1;
            if (rat.y <= 0) rat.y = 1;
            if (rat.y >= MH - 1) rat.y = MH - 2;

            if (rat.x >= 0 && rat.x < MW && rat.y >= 0 && rat.y < MH)
                visit_count[rat.x + rat.y * MW]++;

            // Reset if stuck (visited too many times)
            if (rat.x >= 0 && rat.x < MW && rat.y >= 0 && rat.y < MH) {
                if (visit_count[rat.x + rat.y * MW] > 50) {
                    memcpy(maze, maze_orig, sizeof(maze));
                    memset(visit_count, 0, sizeof(visit_count));
                }
            }
        }
    }

    demo_stop_music();
    demo_free_music(music);
    demo_close(ctx);
    return 0;
}
