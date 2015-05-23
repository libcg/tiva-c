#include "game.h"
#include "disp.h"
#include "ctrl.h"
#include "audio.h"
#include "res/hit_snd.h"
#include "res/goal_snd.h"
#include <math.h>

// Globals

static Game game =
{
    .player.x = 2.f,
    .player.y = 3.f,
    .player.dir = 70.f * M_PI / 180.f,
    .map.tile =
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
    .map.w = sizeof(game.map.tile[0]) / sizeof(game.map.tile[0][0]),
    .map.h = sizeof(game.map.tile) / sizeof(game.map.tile[0])
};

// Local functions

static void gameOnBlockHit(int type)
{
    if (type == 2) // door
        audio_play(&goal_snd, false);
    else
        audio_play(&hit_snd, false);
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

// Exported functions

int gameLocate(int x, int y)
{
    if ((x < 0 || x >= game.map.w) ||
        (y < 0 || y >= game.map.h))
    {
        // Outside the map bounds
        return 1;
    }

    return game.map.tile[y][x];
}

int gameRun()
{
    // Game loop
    while (1)
    {
        audio_process();
        gameLogic();
        dispRender(&game);
    }

    return 0;
}
