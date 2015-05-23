#ifndef __GAME_H
#define __GAME_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH    (320)
#define SCREEN_HEIGHT   (240)
#define M_PI            (3.14159f)
#define COLLIDE_GAP     (0.2f)
#define PLAYER_MOVE     (0.13f)
#define PLAYER_ROT      (0.1f)

typedef struct
{
    float x;
    float y;
    float dir;
} Player;

typedef struct
{
    uint8_t tile[10][10];
    uint8_t w;
    uint8_t h;
    float startX;
    float startY;
    float startDir;
} Map;

typedef struct
{
    Player player;
    Map *map;
    int mapId;
} Game;

int gameLocate(int x, int y);
void gameRun();

#endif // __GAME_H
