#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "grlib/grlib.h"
#include "utils/ustdlib.h"
#include "drivers/frame.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/pinout.h"
#include "drivers/sound.h"
#include "drivers/touch.h"

tDMAControlTable psDMAControlTable[64] __attribute__ ((aligned(1024)));
static long rect_x, rect_y, rect_active, rect_mesg;

static long tsCB(unsigned long ulMessage, long lX, long lY)
{
    rect_x = lX;
    rect_y = lY;
    rect_active = 1;
    rect_mesg = ulMessage * 0x02FF;
    return 0;
}

int
main(void)
{
    tContext sContext;
    uint32_t ui32SysClock;

    //
    // Run from the PLL at 120 MHz.
    //
    ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                           SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                           SYSCTL_CFG_VCO_480), 120000000);

    //
    // Configure the device pins.
    //
    PinoutSet();

    //
    // Initialize the display driver.
    //
    Kentec320x240x16_SSD2119Init(ui32SysClock);

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    //
    // Draw the application frame.
    //
    //FrameDraw(&sContext, "UPMC Rectangle");

    //
    // Configure and enable uDMA
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    ROM_uDMAControlBaseSet(&psDMAControlTable[0]);
    ROM_uDMAEnable();

    //
    // Initialize the touch screen driver and have it route its messages to the
    // widget tree.
    //
    TouchScreenInit(ui32SysClock);
    TouchScreenCallbackSet(tsCB);

    while (1) {
        if (rect_active) {
            g_sKentec320x240x16_SSD2119.pfnPixelDraw(NULL, rect_x-1, rect_y, rect_mesg);
            g_sKentec320x240x16_SSD2119.pfnPixelDraw(NULL, rect_x+1, rect_y, rect_mesg);
            g_sKentec320x240x16_SSD2119.pfnPixelDraw(NULL, rect_x, rect_y-1, rect_mesg);
            g_sKentec320x240x16_SSD2119.pfnPixelDraw(NULL, rect_x, rect_y+1, rect_mesg);
            g_sKentec320x240x16_SSD2119.pfnPixelDraw(NULL, rect_x, rect_y, 0xFFFF);
            rect_active = 0;
        }
        SysCtlDelay(10);
    }
}
