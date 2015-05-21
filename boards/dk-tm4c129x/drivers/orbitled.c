#include "orbitled.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"

#define ORBIT_LED1_BASE GPIO_PORTS_BASE
#define ORBIT_LED2_BASE GPIO_PORTS_BASE
#define ORBIT_LED3_BASE GPIO_PORTQ_BASE
#define ORBIT_LED4_BASE GPIO_PORTE_BASE
#define ORBIT_LED1_PIN  GPIO_PIN_0
#define ORBIT_LED2_PIN  GPIO_PIN_1
#define ORBIT_LED3_PIN  GPIO_PIN_3
#define ORBIT_LED4_PIN  GPIO_PIN_2

typedef struct
{
    int state;
} OrbitLedContext;

static OrbitLedContext ctx;

static void orbitled_update()
{
    ROM_GPIOPinWrite(ORBIT_LED1_BASE, ORBIT_LED1_PIN,
                     ctx.state & ORBIT_LED1 ? ORBIT_LED1_PIN : 0);
    ROM_GPIOPinWrite(ORBIT_LED2_BASE, ORBIT_LED2_PIN,
                     ctx.state & ORBIT_LED2 ? ORBIT_LED2_PIN : 0);
    ROM_GPIOPinWrite(ORBIT_LED3_BASE, ORBIT_LED3_PIN,
                     ctx.state & ORBIT_LED3 ? ORBIT_LED3_PIN : 0);
    ROM_GPIOPinWrite(ORBIT_LED4_BASE, ORBIT_LED4_PIN,
                     ctx.state & ORBIT_LED4 ? ORBIT_LED4_PIN : 0);
}

void orbitled_init()
{
    ctx.state = 0;

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS); // 1, 2
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ); // 3
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // 4

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTS_BASE, GPIO_PIN_0);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTS_BASE, GPIO_PIN_1);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, GPIO_PIN_3);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);

    orbitled_update();
}

void orbitled_on(int leds)
{
    ctx.state |= leds;
    orbitled_update();
}

void orbitled_off(int leds)
{
    ctx.state &= ~leds;
    orbitled_update();
}

void orbitled_toggle(int leds)
{
    ctx.state ^= leds;
    orbitled_update();
}

void orbitled_set(int leds)
{
    ctx.state = leds;
    orbitled_update();
}

int orbitled_state()
{
    return ctx.state;
}
