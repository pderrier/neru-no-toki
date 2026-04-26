// RAT - Original by WondY / ZeN
// Autonomous agent navigating a maze
// Ported from Watcom C Mode 13h to SDL2

#include "demo_framework.h"

#define LOGICAL_W 320
#define LOGICAL_H 200
#define MAZE_W (LOGICAL_W + 2)
#define MAZE_H (LOGICAL_H + 2)
#define MAZE_SIZE (MAZE_W * MAZE_H)
#define LOGICAL_SIZE (LOGICAL_W * LOGICAL_H)
#define MAZE_IDX(x, y) ((y) * MAZE_W + (x))
#define MAZE_AT_LOGICAL(x, y) maze[MAZE_IDX((x) + 1, (y) + 1)]

typedef struct { int x, y; } Mobile;

short maze[MAZE_SIZE];      // maze layout
short maze_orig[MAZE_SIZE]; // original copy
unsigned int visit_count[LOGICAL_SIZE];
unsigned char display[LOGICAL_SIZE];

Mobile rat;
int deplx = 1, deply = 0;

void generate_maze(void) {
    // Generate a procedural maze
    memset(maze, 0, sizeof(maze));

    // Walls
    for (int y = 0; y < MAZE_H; y++)
        for (int x = 0; x < MAZE_W; x++)
            maze[MAZE_IDX(x, y)] = 1; // default: walkable

    // Border walls
    for (int x = 0; x < MAZE_W; x++) {
        maze[MAZE_IDX(x, 0)] = -1;
        maze[MAZE_IDX(x, MAZE_H - 1)] = -1;
    }
    for (int y = 0; y < MAZE_H; y++) {
        maze[MAZE_IDX(0, y)] = -1;
        maze[MAZE_IDX(MAZE_W - 1, y)] = -1;
    }

    // Internal walls (create a simple maze pattern)
    for (int y = 20; y < 180; y += 40) {
        for (int x = 20; x < 300; x++) {
            if ((x % 80) < 60) {
                maze[MAZE_IDX(x + 1, y + 1)] = -1;
            }
        }
    }

    for (int x = 40; x < 280; x += 60)
        for (int y = 40; y < 180; y++)
            if ((y % 60) < 40)
                maze[MAZE_IDX(x + 1, y + 1)] = -1;

    // Some random obstacles
    srand(42);
    for (int i = 0; i < 200; i++) {
        int x = rand() % 300 + 10;
        int y = rand() % 180 + 10;
        for (int dy = 0; dy < 5; dy++)
            for (int dx = 0; dx < 5; dx++)
                if (x + dx < LOGICAL_W && y + dy < LOGICAL_H)
                    maze[MAZE_IDX(x + dx + 1, y + dy + 1)] = -1;
    }

    memcpy(maze_orig, maze, sizeof(maze));
}

void calc_movement(void) {
    int score_r = 0, score_l = 0, score_d = 0, score_u = 0;
    int x, y;
    int best, count, pick;
    int candidates[4];

    // Right ray
    y = rat.y + 1;
    for (x = rat.x + 2; x <= LOGICAL_W; x++) {
        if (maze[MAZE_IDX(x, y)] != 1) break;
        score_r++;
    }
    // Left ray
    for (x = rat.x; x >= 1; x--) {
        if (maze[MAZE_IDX(x, y)] != 1) break;
        score_l++;
    }
    // Down ray
    x = rat.x + 1;
    for (y = rat.y + 2; y <= LOGICAL_H; y++) {
        if (maze[MAZE_IDX(x, y)] != 1) break;
        score_d++;
    }
    // Up ray
    for (y = rat.y; y >= 1; y--) {
        if (maze[MAZE_IDX(x, y)] != 1) break;
        score_u++;
    }

    // Keep some momentum, avoid instant U-turn.
    if (deplx == 1) score_r += 2;
    if (deplx == -1) score_l += 2;
    if (deply == 1) score_d += 2;
    if (deply == -1) score_u += 2;
    if (deplx == 1) score_l -= 2;
    if (deplx == -1) score_r -= 2;
    if (deply == 1) score_u -= 2;
    if (deply == -1) score_d -= 2;

    best = -32768;
    count = 0;

    // Right
    if (rat.x + 1 < LOGICAL_W && MAZE_AT_LOGICAL(rat.x + 1, rat.y) == 1) {
        if (score_r > best) { best = score_r; count = 0; candidates[count++] = 0; }
        else if (score_r == best) candidates[count++] = 0;
    }
    // Left
    if (rat.x - 1 >= 0 && MAZE_AT_LOGICAL(rat.x - 1, rat.y) == 1) {
        if (score_l > best) { best = score_l; count = 0; candidates[count++] = 1; }
        else if (score_l == best) candidates[count++] = 1;
    }
    // Down
    if (rat.y + 1 < LOGICAL_H && MAZE_AT_LOGICAL(rat.x, rat.y + 1) == 1) {
        if (score_d > best) { best = score_d; count = 0; candidates[count++] = 2; }
        else if (score_d == best) candidates[count++] = 2;
    }
    // Up
    if (rat.y - 1 >= 0 && MAZE_AT_LOGICAL(rat.x, rat.y - 1) == 1) {
        if (score_u > best) { best = score_u; count = 0; candidates[count++] = 3; }
        else if (score_u == best) candidates[count++] = 3;
    }

    if (count <= 0) {
        deplx = 0;
        deply = 0;
        return;
    }

    pick = candidates[rand() % count];
    if (pick == 0) { deplx = 1;  deply = 0; return; }
    if (pick == 1) { deplx = -1; deply = 0; return; }
    if (pick == 2) { deplx = 0;  deply = 1; return; }
    deplx = 0;
    deply = -1;
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
                int idx = MAZE_IDX(x + 1, y + 1);
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
