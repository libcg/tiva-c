#ifndef __ORBITLED_H
#define __ORBITLED_H

#define ORBIT_LED1  (1)
#define ORBIT_LED2  (2)
#define ORBIT_LED3  (4)
#define ORBIT_LED4  (8)

void orbitled_init();
void orbitled_on(int leds);
void orbitled_off(int leds);
void orbitled_toggle(int leds);
void orbitled_set(int leds);
int orbitled_state();

#endif // __ORBITLED_H
