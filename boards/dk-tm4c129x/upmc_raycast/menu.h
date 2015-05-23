#ifndef __MENU_H
#define __MENU_H

typedef enum
{
	MENU_HOME = 0,
	MENU_OPTIONS,
	MENU_NEXTLEVEL
} MenuState;

void menuInit();
void menuSetState(int state);
void menuRun();

#endif // __MENU_H
