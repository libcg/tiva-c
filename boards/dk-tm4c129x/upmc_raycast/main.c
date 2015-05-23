#include "game.h"
#include "menu.h"
#include "ctrl.h"
#include "audio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/pinout.h"
#include "drivers/touch.h"
#include "drivers/orbitled.h"

// Globals

static bool g_inGame;

// Local functions

static long touchscreenCallback(unsigned long msg, long x, long y)
{
    bool touch = (msg == WIDGET_MSG_PTR_MOVE || msg == WIDGET_MSG_PTR_DOWN);

    // Dispatch the message to widget and game components
    if (g_inGame)
        ctrlTouchEvent(touch, x, y);
    else
        WidgetPointerMessage(msg, x, y);

    return 0;
}

// Exported functions

int main()
{
    static tDMAControlTable DMACtrlTable[64] __attribute__ ((aligned(1024)));
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
    ROM_uDMAControlBaseSet(&DMACtrlTable);
    ROM_uDMAEnable();

    // Initialize the touch screen driver.
    TouchScreenInit(ui32SysClock);
    TouchScreenCallbackSet(touchscreenCallback);

    // Initialize audio
    audio_init(ui32SysClock);

    // Initialize LEDs
    orbitled_init();

    menuInit();

    while (1) {
        g_inGame = false;
        menuRun();

        g_inGame = true;
        gameRun();
    }

    return 0;
}
