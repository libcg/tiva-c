#ifndef __GAME_H
#define __GAME_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH    (320)
#define SCREEN_HEIGHT   (240)
#define M_PI            (3.14159f)
#define FIELD_OF_VIEW   (70.f * M_PI / 180.f)
#define COLLIDE_GAP     (0.2f)
#define TEX_SIZE        (32)
#define PLAYER_MOVE     (0.13f)
#define PLAYER_ROT      (0.1f)
#define CEILING_COLOR   (0x7BEF) // lite gray
#define FLOOR_COLOR     (0x2965) // dark gray

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
} Map;

typedef struct
{
    Player player;
    Map map;
} Game;

int gameLocate(int x, int y);
int gameRun();

#endif // __GAME_H
