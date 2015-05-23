#include "ctrl.h"
#include <math.h>

// Globals

static bool g_tsTouch;
static int g_tsX;
static int g_tsY;

// Exported functions

void ctrlLogic(Game *game)
{
    if (g_tsTouch) {
        if (g_tsX > 2 * SCREEN_WIDTH / 3) {
            // Turn right
            game->player.dir += PLAYER_ROT;
        }
        else if (g_tsX < 1 * SCREEN_WIDTH / 3) {
            // Turn left
            game->player.dir -= PLAYER_ROT;
        }
        if (g_tsY > 2 * SCREEN_HEIGHT / 3) {
            // Move forward
            game->player.x -= PLAYER_MOVE * cosf(game->player.dir);
            game->player.y -= PLAYER_MOVE * sinf(game->player.dir);
        }
        else if (g_tsY < 1 * SCREEN_HEIGHT / 3) {
            // Move backward
            game->player.x += PLAYER_MOVE * cosf(game->player.dir);
            game->player.y += PLAYER_MOVE * sinf(game->player.dir);
        }
    }
}

void ctrlTouchEvent(bool touch, int x, int y)
{
    g_tsTouch = touch;
    g_tsX = x;
    g_tsY = y;
}
