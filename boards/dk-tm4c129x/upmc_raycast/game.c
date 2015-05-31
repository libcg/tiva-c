#include "game.h"
#include "disp.h"
#include "ctrl.h"
#include "audio.h"
#include "menu.h"
#include "config.h"
#include <math.h>
#include "drivers/orbitled.h"

#ifdef USE_SOUND
#include "res/hit_snd.h"
#include "res/goal_snd.h"
#include "res/timesup_snd.h"
#endif

// Globals

static Map map[] =
{
    { // map0
        .tile =
        {
            {1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
            {0, 0, 0, 0, 0, 1, 0, 0, 0, 2},
            {0, 0, 0, 0, 1, 0, 0, 0, 1, 1},
            {0, 0, 0, 1, 3, 0, 3, 0, 0, 0},
            {0, 0, 0, 1, 3, 0, 3, 0, 0, 1},
            {0, 0, 0, 1, 3, 0, 3, 0, 0, 0},
            {0, 0, 0, 1, 1, 0, 3, 0, 0, 1},
            {4, 0, 0, 0, 0, 0, 1, 0, 0, 0},
            {4, 0, 0, 0, 0, 0, 1, 0, 0, 1},
            {0, 4, 4, 4, 4, 0, 0, 0, 1, 0}
        },
        .w = 10,
        .h = 10,
        .startX = 2.f,
        .startY = 3.f,
        .startDir = 70.f * M_PI / 180.f
    },
    { // map1
        .tile =
        {
            {0, 1, 0, 1, 0, 3, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 3, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 3, 0, 0, 0, 0},
            {1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
            {4, 2, 4, 1, 3, 0, 3, 3, 3, 0},
            {4, 0, 0, 1, 3, 3, 3, 0, 0, 0},
            {4, 0, 0, 1, 3, 0, 0, 0, 3, 3},
            {4, 0, 0, 0, 0, 0, 4, 0, 0, 3},
            {4, 0, 0, 0, 0, 0, 4, 0, 0, 0},
            {4, 4, 4, 4, 4, 4, 4, 0, 1, 0}
        },
        .w = 10,
        .h = 10,
        .startX = 0.5f,
        .startY = 0.5f,
        .startDir = 90.f * M_PI / 180.f
    },
    { // map2
        .tile =
        {
            {4, 4, 4, 4, 4, 4, 4, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 3, 0},
            {3, 0, 0, 0, 4, 4, 4, 0, 0, 0},
            {3, 0, 0, 4, 0, 0, 0, 1, 1, 0},
            {3, 0, 4, 2, 0, 0, 0, 0, 0, 0},
            {3, 0, 4, 2, 0, 0, 0, 0, 0, 0},
            {3, 0, 0, 4, 0, 0, 0, 1, 1, 0},
            {3, 0, 0, 0, 4, 4, 4, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 3, 0},
            {4, 4, 4, 4, 4, 4, 4, 0, 0, 0}
        },
        .w = 10,
        .h = 10,
        .startX = 4.5f,
        .startY = 5.f,
        .startDir = 0.f * M_PI / 180.f
    },
};

static Game game =
{
    .mapId = 0
};

static bool g_exit;

// Local functions

static void gameOnBlockHit(int type)
{
    if (type == 2) { // door
#ifdef USE_SOUND
        audio_play(&goal_snd, false);
#endif
        menuSetState(MENU_NEXTLEVEL);
        g_exit = true;
    }
    else {
#ifdef USE_SOUND
        audio_play(&hit_snd, false);
#endif
    }
}

static void gameCollision(float opx, float opy)
{
    static bool collided_x = false;
    static bool collided_y = false;

    float px = game.player.x;
    float py = game.player.y;
    int fpx = floorf(px);
    int fpy = floorf(py);
    bool colliding_x = false;
    bool colliding_y = false;
    int collide_type_x = 0;
    int collide_type_y = 0;
    int type;

    // Check for x axis collisions
    if      ((type = gameLocate(floorf(px + COLLIDE_GAP), fpy))) {
        colliding_x = true, collide_type_x = type;
        game.player.x = fpx - COLLIDE_GAP + 1;
    }
    else if ((type = gameLocate(floorf(px - COLLIDE_GAP), fpy))) {
        colliding_x = true, collide_type_x = type;
        game.player.x = fpx + COLLIDE_GAP;
    }

    // Check for y axis collisions
    if      ((type = gameLocate(fpx, floorf(py + COLLIDE_GAP)))) {
        colliding_y = true, collide_type_y = type;
        game.player.y = fpy - COLLIDE_GAP + 1;
    }
    else if ((type = gameLocate(fpx, floorf(py - COLLIDE_GAP)))) {
        colliding_y = true, collide_type_y = type;
        game.player.y = fpy + COLLIDE_GAP;
    }

    // Check if we're inside a wall
    if ((type = gameLocate(fpx, fpy))) {
        colliding_x = true, collide_type_x = type;
        colliding_y = true, collide_type_y = type;
        game.player.x = opx, game.player.y = opy;
    }

    // Take action when the player hits a block
    if (colliding_x && !collided_x)
        gameOnBlockHit(collide_type_x);
    if (colliding_y && !collided_y)
        gameOnBlockHit(collide_type_y);

    collided_x = colliding_x;
    collided_y = colliding_y;
}

static void gameLogic()
{
    float opx = game.player.x;
    float opy = game.player.y;

    ctrlLogic(&game);
    gameCollision(opx, opy);
}

static void gameInitMap()
{
    game.map = &map[game.mapId];
    game.player.x = game.map->startX;
    game.player.y = game.map->startY;
    game.player.dir = game.map->startDir;
    game.timeLeft = TIME_TOTAL;
}

// Exported functions

int gameLocate(int x, int y)
{
    if ((x < 0 || x >= game.map->w) ||
        (y < 0 || y >= game.map->h)) {
        // Outside the map bounds
        return 1;
    }

    return game.map->tile[y][x];
}

void gameRun()
{
    gameInitMap();
    orbitled_set(ORBIT_LED_ALL);

    g_exit = false;

    // Game loop
    while (!g_exit) {
        audio_process();
        gameLogic();
        dispRender(&game);
    }

    if (game.timeLeft == 0) {
        // Time's up!
        game.mapId = 0;
        menuSetState(MENU_TIMESUP);
#ifdef USE_SOUND
        audio_play(&timesup_snd, false);
#endif
    }
    else {
        // Go to next map
        game.mapId++;

        if (game.mapId >= sizeof(map) / sizeof(Map)) {
            game.mapId = 0;
            menuSetState(MENU_END);
        }
    }

    orbitled_set(ORBIT_LED_NONE);
}

void gameTick()
{
    game.timeLeft--;

    if (game.timeLeft == 0) {
        g_exit = true;
    }
    else {
        int dot = (TIME_TOTAL - game.timeLeft) * 4 / TIME_TOTAL;

        orbitled_set(((ORBIT_LED_ALL >> dot) ^ (game.timeLeft & 1)) << dot);
    }
}
