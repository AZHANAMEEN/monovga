#ifndef APPLICATIONHANDLER_H
#define APPLICATIONHANDLER_H
typedef enum {
    SCREEN_HOME,
    SCREEN_SNAKE,
    SCREEN_TETRIS,
    SCREEN_SETTINGS,
    SCREEN_MVPAINT
} ScreenState;
void homeloop(void) ;
void homesetup(void);
void switchsetup(void);
void updateScreen(void);

#endif