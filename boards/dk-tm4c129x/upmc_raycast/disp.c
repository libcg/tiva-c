#include "disp.h"
#include "game.h"
#include <stdlib.h>
#include <math.h>
#include "grlib/grlib.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "res/rock_tex.h"
#include "res/door_tex.h"
#include "res/handle_tex.h"
#include "res/wood_tex.h"

// Globals

static uint16_t *g_tex[] =
{
    NULL,
    (uint16_t*)&rock_tex_data,
    (uint16_t*)&door_tex_data,
    (uint16_t*)&handle_tex_data,
    (uint16_t*)&wood_tex_data
};

// Local functions

static void dispDrawColumn(Game *game, int x, int height, int type, float tx)
{
    uint16_t *tex = g_tex[type];

    // Ceiling
    g_sKentec320x240x16_SSD2119.pfnLineDrawV(
        NULL, x,
        0, (SCREEN_HEIGHT - height) / 2,
        CEILING_COLOR
    );

    // Wall
    if (tex) {
        // Draw a texture slice
        Kentec320x240x16_SSD2119TexDrawV(
            NULL, x,
            (SCREEN_HEIGHT - height) / 2, (SCREEN_HEIGHT + height) / 2,
            &tex[(int)(tx * TEX_SIZE) * TEX_SIZE], TEX_SIZE
        );
    }
    else {
        // Draw a solid white slice
        g_sKentec320x240x16_SSD2119.pfnLineDrawV(
            NULL, x,
            (SCREEN_HEIGHT - height) / 2, (SCREEN_HEIGHT + height) / 2,
            0xFFFF
        );
    }

    // Floor
    g_sKentec320x240x16_SSD2119.pfnLineDrawV(
        NULL, x,
        (SCREEN_HEIGHT + height) / 2, SCREEN_HEIGHT,
        FLOOR_COLOR
    );
}

static float dispRaycast(Game *game, int *type, float *tx, float angle)
{
    float x = game->player.x;
    float y = game->player.y;

    if ((gameLocate(floorf(x), floorf(y))) != 0)
    {
        *type = 0;
        *tx = 0.f;
        return 0.0001f;
    }

    // Precompute
    float vsin = sinf(angle);
    float vcos = cosf(angle);
    float vtan = vsin / vcos;

    // Calculate increments
    int incix = (vcos > 0.f) - (vcos < 0.f);
    int inciy = (vsin > 0.f) - (vsin < 0.f);
    float incfx = inciy / vtan;
    float incfy = incix * vtan;

    // Calculate start position
    int ix = x + (incix > 0);
    int iy = y + (inciy > 0);
    float fx = x + incfx * fabs(floor(y) + (inciy > 0) - y);
    float fy = y + incfy * fabs(floor(x) + (incix > 0) - x);

    // Find the first colliding tile in each direction
    while (incix && gameLocate(ix - (incix < 0), fy) == 0)
    {
        ix += incix;
        fy += incfy;
    }
    while (inciy && gameLocate(fx, iy - (inciy < 0)) == 0)
    {
        fx += incfx;
        iy += inciy;
    }

    // Find the shortest ray
    float dx = (incix) ? ((ix - x) / vcos) : 0xFFFF;
    float dy = (inciy) ? ((iy - y) / vsin) : 0xFFFF;

    if (dx < dy)
    {
        *type = gameLocate(ix - (incix < 0), floorf(fy));
        *tx = fy - floorf(fy);
        if (incix < 0)
            *tx = 1 - *tx;
        return dx;
    }
    else
    {
        *type = gameLocate(floorf(fx), iy - (inciy < 0));
        *tx = fx - floorf(fx);
        if (inciy > 0)
            *tx = 1 - *tx;
        return dy;
    }
}

// Exported functions

void dispRender(Game *game)
{
    float angle = game->player.dir - FIELD_OF_VIEW / 2.f;

    // Cast a ray for each pixel column and draw a colored wall slice
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
        float dist;
        int type;
        float tx;

        dist = dispRaycast(game, &type, &tx, angle);
        dist *= cosf(game->player.dir - angle);
        angle += FIELD_OF_VIEW / SCREEN_WIDTH;

        dispDrawColumn(game, i, SCREEN_HEIGHT / dist, type, tx);
    }
}
