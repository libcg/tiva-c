#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "utils/ustdlib.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/pinout.h"
#include "drivers/touch.h"
#include "audio.h"

#define SCREEN_WIDTH    (320)
#define SCREEN_HEIGHT   (240)
#define PI              (3.14159f)
#define FIELD_OF_VIEW   (70.f * PI / 180.f)
#define COLLIDE_GAP     (0.2f)

typedef struct
{
    bool touch;
    int x;
    int y;
} Touchscreen;

typedef struct
{
    float x;
    float y;
    float dir;
} Player;

typedef struct
{
    int tile[10][10];
    int w;
    int h;
} Map;

typedef struct
{
    Player player;
    Map map;
} Game;

static Touchscreen ts =
{
    .touch = false,
    .x = 0,
    .y = 0
};

static int
gameLocate(Game *game, int x, int y)
{
    if ((x < 0 || x >= game->map.w) ||
        (y < 0 || y >= game->map.h))
    {
        // Outside the map bounds
        return 1;
    }

    return game->map.tile[y][x];
}

static void
gameCollision(Game *game)
{
    float px = game->player.x;
    float py = game->player.y;
    int fpx = floorf(px);
    int fpy = floorf(py);

    if      (gameLocate(game, floorf(px + COLLIDE_GAP), fpy))
        game->player.x = fpx - COLLIDE_GAP + 1;
    else if (gameLocate(game, floorf(px - COLLIDE_GAP), fpy))
        game->player.x = fpx + COLLIDE_GAP;
    if      (gameLocate(game, fpx, floorf(py + COLLIDE_GAP)))
        game->player.y = fpy - COLLIDE_GAP + 1;
    else if (gameLocate(game, fpx, floorf(py - COLLIDE_GAP)))
        game->player.y = fpy + COLLIDE_GAP;
}

static float
gameRaycast(Game *game, int *type, float angle)
{
    float x = game->player.x;
    float y = game->player.y;

    if ((*type = gameLocate(game, floorf(x), floorf(y))) != 0)
    {
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
    while (incix && gameLocate(game, ix - (incix < 0), fy) == 0)
    {
        ix += incix;
        fy += incfy;
    }
    while (inciy && gameLocate(game, fx, iy - (inciy < 0)) == 0)
    {
        fx += incfx;
        iy += inciy;
    }

    // Find the shortest ray
    float dx = (incix) ? ((ix - x) / vcos) : 0xFFFF;
    float dy = (inciy) ? ((iy - y) / vsin) : 0xFFFF;

    if (dx < dy)
    {
        *type = 1;
        return dx;
    }
    else
    {
        *type = 2;
        return dy;
    }
}

static void
gameLogic(Game *game)
{
    if (ts.touch) {
        if (ts.x > 2 * SCREEN_WIDTH / 3) {
            // Turn right
            game->player.dir += 0.1f;
        }
        else if (ts.x < 1 * SCREEN_WIDTH / 3) {
            // Turn left
            game->player.dir -= 0.1f;
        }
        if (ts.y > 2 * SCREEN_HEIGHT / 3) {
            // Move forward
            game->player.x -= 0.13f * cosf(game->player.dir);
            game->player.y -= 0.13f * sinf(game->player.dir);
        }
        else if (ts.y < 1 * SCREEN_HEIGHT / 3) {
            // Move backward
            game->player.x += 0.13f * cosf(game->player.dir);
            game->player.y += 0.13f * sinf(game->player.dir);
        }

        gameCollision(game);
    }
}

static void
gameDrawWall(Game *game, int x, int height, int color)
{
    if (height > SCREEN_HEIGHT)
    {
        height = SCREEN_HEIGHT;
    }

    // Top background
    g_sKentec320x240x16_SSD2119.pfnLineDrawV
    (
        NULL, x,
        0, (SCREEN_HEIGHT - height) / 2,
        0x0000
    );
    // Wall
    g_sKentec320x240x16_SSD2119.pfnLineDrawV
    (
        NULL, x,
        (SCREEN_HEIGHT - height) / 2, (SCREEN_HEIGHT + height) / 2,
        color
    );
    // Bottom background
    g_sKentec320x240x16_SSD2119.pfnLineDrawV
    (
        NULL, x,
        (SCREEN_HEIGHT + height) / 2, SCREEN_HEIGHT,
        0x0000
    );
}

static void
gameRender(Game *game)
{
    float angle = game->player.dir - FIELD_OF_VIEW / 2.f;

    // Cast a ray for each pixel column and draw a colored wall slice
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
        float dist;
        int type;
        int color = 0x0000;

        dist = gameRaycast(game, &type, angle);
        dist *= cosf(game->player.dir - angle);
        angle += FIELD_OF_VIEW / SCREEN_WIDTH;

        switch (type)
        {
            case 1: color = 0xFFFF; break;
            case 2: color = 0x6232; break;
        }
        gameDrawWall(game, i, SCREEN_HEIGHT / dist, color);
    }
}

static int
gameRun(void)
{
    // Create and initialize a game state
    static Game game =
    {
        .player.x = 2.f,
        .player.y = 3.f,
        .player.dir = 70.f * PI / 180.f,
        .map.tile =
        {
            {0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
            {0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
            {0, 0, 0, 1, 1, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 1, 0, 1, 0, 0, 1},
            {0, 0, 0, 1, 1, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 1, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 1, 1, 0, 0, 0, 1, 0}
        },
        .map.w = sizeof(game.map.tile[0]) / sizeof(game.map.tile[0][0]),
        .map.h = sizeof(game.map.tile) / sizeof(game.map.tile[0])
    };

    // Game loop
    while (1)
    {
        audio_process();
        gameLogic(&game);
        gameRender(&game);
    }

    return 0;
}

static long
touchscreenCallback(unsigned long ulMessage, long lX, long lY)
{
    WidgetPointerMessage(ulMessage, lX, lY);

    // Register a touch event
    ts.touch = (ulMessage == WIDGET_MSG_PTR_MOVE || ulMessage == WIDGET_MSG_PTR_DOWN);
    ts.x = lX;
    ts.y = lY;
    return 0;
}

int
main(void)
{
    static tDMAControlTable psDMAControlTable[64] __attribute__ ((aligned(1024)));
    tContext sContext;
    uint32_t ui32SysClock;

    // Run from the PLL at 120 MHz.
    ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                           SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                           SYSCTL_CFG_VCO_480), 120000000);

    // Configure the device pins.
    PinoutSet();

    // Initialize the display driver.
    Kentec320x240x16_SSD2119Init(ui32SysClock);

    // Initialize the graphics context.
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    // Configure and enable uDMA
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    ROM_uDMAControlBaseSet(&psDMAControlTable[0]);
    ROM_uDMAEnable();

    // Initialize the touch screen driver.
    TouchScreenInit(ui32SysClock);
    TouchScreenCallbackSet(touchscreenCallback);

    // Initialize audio
    audio_init(ui32SysClock);

    menuInit();
    menuRun();

    return gameRun();
}
