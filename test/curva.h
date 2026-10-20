#ifndef CURVA_H
#define CURVA_H


#include <avr/pgmspace.h>
#include <stdint.h>
#define cxst 16
#define cyen 112
bool coor1;
bool coor2;
typedef enum {
    T_PENCIL,
    T_LINE,
    T_CIRCLE,
    T_SQUARE,
    T_FILL,
    T_ERASE,
    T_SQ_ERASE
} tool;
static int x1;
static int y1;
static int x2;
static int y2;
#define TOOL_W 8
#define TOOL_H 8
// Pencil: diagonal shaft and pointed tip
static const uint8_t TOOL_PENCIL[8][8] PROGMEM = {
    {0,0,0,0,0,0,3,0},
    {0,0,0,0,0,3,2,0},
    {0,0,0,0,3,2,0,0},
    {0,0,0,3,2,0,0,0},
    {0,0,3,2,0,0,0,0},
    {0,3,2,0,0,0,0,0},
    {3,2,0,0,0,0,0,0},
    {2,0,0,0,0,0,0,0}
};

// Line: diagonal stroke with a bright core
static const uint8_t TOOL_LINE[8][8] PROGMEM = {
    {0,0,0,0,0,0,0,3},
    {0,0,0,0,0,0,3,2},
    {0,0,0,0,0,3,2,0},
    {0,0,0,0,3,2,0,0},
    {0,0,0,3,2,0,0,0},
    {0,0,3,2,0,0,0,0},
    {0,3,2,0,0,0,0,0},
    {3,2,0,0,0,0,0,0}
};

// Circle: hollow ellipse
static const uint8_t TOOL_CIRCLE[8][8] PROGMEM = {
    {0,0,2,3,3,2,0,0},
    {0,2,0,0,0,0,2,0},
    {2,0,0,0,0,0,0,2},
    {3,0,0,0,0,0,0,3},
    {3,0,0,0,0,0,0,3},
    {2,0,0,0,0,0,0,2},
    {0,2,0,0,0,0,2,0},
    {0,0,2,3,3,2,0,0}
};

// Square: hollow rectangle
static const uint8_t TOOL_SQUARE[8][8] PROGMEM = {
    {3,3,3,3,3,3,3,3},
    {3,0,0,0,0,0,0,3},
    {3,0,0,0,0,0,0,3},
    {3,0,0,0,0,0,0,3},
    {3,0,0,0,0,0,0,3},
    {3,0,0,0,0,0,0,3},
    {3,0,0,0,0,0,0,3},
    {3,3,3,3,3,3,3,3}
};

// Fill: bucket-like silhouette with pixels pouring out
static const uint8_t TOOL_FILL[8][8] PROGMEM = {
    {0,0,3,3,0,0,0,0},
    {0,3,2,2,3,0,0,0},
    {3,2,2,2,2,3,0,0},
    {0,3,2,2,3,0,0,0},
    {0,0,3,3,0,0,0,0},
    {0,0,0,2,0,0,0,0},
    {0,0,2,0,2,0,0,0},
    {0,0,0,0,0,0,0,0}
};

// Eraser: diagonal block with two-tone face
static const uint8_t TOOL_ERASE[8][8] PROGMEM = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,3,3,0},
    {0,0,0,0,3,2,3,0},
    {0,0,0,3,2,2,3,0},
    {0,0,3,2,2,3,0,0},
    {0,3,2,2,3,0,0,0},
    {3,2,2,3,0,0,0,0},
    {3,3,3,0,0,0,0,0}
};

// Square erase: outlined selection box with an X
static const uint8_t TOOL_SQ_ERASE[8][8] PROGMEM = {
    {3,3,3,3,3,3,3,3},
    {3,2,0,0,0,0,2,3},
    {3,0,3,0,0,3,0,3},
    {3,0,0,3,3,0,0,3},
    {3,0,0,3,3,0,0,3},
    {3,0,3,0,0,3,0,3},
    {3,2,0,0,0,0,2,3},
    {3,3,3,3,3,3,3,3}
};

void masterloop(void);
void setuphandle(void);
void toolhandler(void);
#endif