// RAT - Original by WondY / ZeN
// Autonomous agent navigating a maze
// Ported from Watcom C Mode 13h to SDL2

#include "demo_framework.h"

#define LOGICAL_W 320
#define LOGICAL_H 200
#define MAZE_W (LOGICAL_W + 2)
#define MAZE_H (LOGICAL_H + 2)

#define DROITE  1
#define GAUCHE -1
#define HAUT   -1
#define BAS     1

typedef struct { int x, y; } Mobile;

short maze[MAZE_W * MAZE_H];      // maze layout
short maze_orig[MAZE_W * MAZE_H]; // original copy
unsigned int visit_count[LOGICAL_W * LOGICAL_H];
unsigned char display[LOGICAL_W * LOGICAL_H];

Mobile rat;
int deplx = 1, deply = 0;

static inline int maze_idx(int x, int y) {
    return y * MAZE_W + x;
}

void generate_maze(void) {
    // Generate a procedural maze
    memset(maze, 0, sizeof(maze));

    // Walls
    for (int y = 0; y < MAZE_H; y++)
        for (int x = 0; x < MAZE_W; x++)
            maze[maze_idx(x, y)] = 1; // default: walkable

    // Border walls
    for (int x = 0; x < MAZE_W; x++) {
        maze[maze_idx(x, 0)] = -1;
        maze[maze_idx(x, MAZE_H - 1)] = -1;
    }
    for (int y = 0; y < MAZE_H; y++) {
        maze[maze_idx(0, y)] = -1;
        maze[maze_idx(MAZE_W - 1, y)] = -1;
    }

    // Internal walls (create a simple maze pattern)
    for (int y = 20; y < 180; y += 40) {
        for (int x = 20; x < 300; x++) {
            if ((x % 80) < 60) {
                maze[maze_idx(x + 1, y + 1)] = -1;
            }
        }
    }

    for (int x = 40; x < 280; x += 60)
        for (int y = 40; y < 180; y++)
            if ((y % 60) < 40)
                maze[maze_idx(x + 1, y + 1)] = -1;

    // Some random obstacles
    srand(42);
    for (int i = 0; i < 200; i++) {
        int x = rand() % 300 + 10;
        int y = rand() % 180 + 10;
        for (int dy = 0; dy < 5; dy++)
            for (int dx = 0; dx < 5; dx++)
                if (x + dx < LOGICAL_W && y + dy < LOGICAL_H)
                    maze[maze_idx(x + dx + 1, y + dy + 1)] = -1;
    }

    memcpy(maze_orig, maze, sizeof(maze));
}

void calc_movement(void) {
    int td = 0, tg = 0, th = 0, tb = 0;

    // Look in each direction for walls
    for (int x = rat.x + 1; x <= LOGICAL_W; x++) {
        int idx = maze_idx(x + 1, rat.y + 1);
        if (maze[idx] == 1 || td <= 0) td += maze[idx];
        else break;
    }
    for (int x = rat.x - 1; x >= -1; x--) {
        int idx = maze_idx(x + 1, rat.y + 1);
        if (maze[idx] == 1 || tg <= 0) tg += maze[idx];
        else break;
    }
    for (int y = rat.y + 1; y <= LOGICAL_H; y++) {
        int idx = maze_idx(rat.x + 1, y + 1);
        if (maze[idx] == 1 || tb <= 0) tb += maze[idx];
        else break;
    }
    for (int y = rat.y - 1; y >= -1; y--) {
        int idx = maze_idx(rat.x + 1, y + 1);
        if (maze[idx] == 1 || th <= 0) th += maze[idx];
        else break;
    }

    // Scale vertical to match aspect ratio
    tb = (int)((float)tb * (float)LOGICAL_W / (float)LOGICAL_H);
    th = (int)((float)th * (float)LOGICAL_W / (float)LOGICAL_H);

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

    DemoContext *ctx = demo_init("RAT - WondY / ZeN", LOGICAL_W, LOGICAL_H);
    if (!ctx) return 1;

    Mix_Music *music = demo_load_music("data/ZINZIN.XM");
    demo_play_music(music);

    generate_maze();
    memset(visit_count, 0, sizeof(visit_count));
    rat.x = 10; rat.y = 75;

    while (demo_poll(ctx)) {
        // Render maze
        for (int y = 0; y < LOGICAL_H; y++) {
            for (int x = 0; x < LOGICAL_W; x++) {
                int i = y * LOGICAL_W + x;
                int idx = maze_idx(x + 1, y + 1);
                if (maze_orig[idx] == -1)
                    display[i] = 128; // wall
                else
                    display[i] = 0;   // floor
            }
        }

        // Draw rat trail (heat map)
        for (int y = 0; y < LOGICAL_H; y++) {
            for (int x = 0; x < LOGICAL_W; x++) {
                unsigned int v = visit_count[x + y * LOGICAL_W];
                if (v > 0) {
                    int c = (int)(v * 4);
                    if (c > 60) c = 60;
                    display[y * LOGICAL_W + x] = (unsigned char)(30 + c);
                }
            }
        }

        // Draw rat
        if (rat.x >= 0 && rat.x < LOGICAL_W - 1 && rat.y >= 0 && rat.y < LOGICAL_H - 1) {
            display[rat.y * LOGICAL_W + rat.x] = 255;
            display[rat.y * LOGICAL_W + rat.x + 1] = 255;
            display[(rat.y + 1) * LOGICAL_W + rat.x] = 255;
            display[(rat.y + 1) * LOGICAL_W + rat.x + 1] = 255;
        }

        // Render to screen
        for (int i = 0; i < LOGICAL_W * LOGICAL_H; i++) {
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
            if (rat.x >= LOGICAL_W) rat.x = LOGICAL_W - 1;
            if (rat.y <= 0) rat.y = 1;
            if (rat.y >= LOGICAL_H - 1) rat.y = LOGICAL_H - 2;

            if (rat.x >= 0 && rat.x < LOGICAL_W && rat.y >= 0 && rat.y < LOGICAL_H)
                visit_count[rat.x + rat.y * LOGICAL_W]++;

            // Reset if stuck (visited too many times)
            if (rat.x >= 0 && rat.x < LOGICAL_W && rat.y >= 0 && rat.y < LOGICAL_H) {
                if (visit_count[rat.x + rat.y * LOGICAL_W] > 50) {
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
