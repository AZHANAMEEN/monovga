#ifndef APPLICATIONHANDLER_H
#define APPLICATIONHANDLER_H
#include <avr/pgmspace.h>
typedef enum {
    SCREEN_HOME,
    SCREEN_SNAKE,
    SCREEN_TETRIS,
    SCREEN_SETTINGS,
    SCREEN_CURVA
} ScreenState;
void homeloop(void) ;
void homesetup(void);
void switchsetup(void);
void updateScreen(void);
void taskbar(void);
void uidrawroutine(void);
void stateTick(void);
void draw_xval(void);
extern void update_cursor(void);
void hide_cursor(void);
void invalidate_cursor(void);
extern int cursorx;
extern int cursory;
static int ox;
static int oy;
static int dx;

#endif