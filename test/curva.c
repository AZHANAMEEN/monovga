#include "applicationhandler.h"
#include "snake.h"
#include "drfx.h"
#include <avr/pgmspace.h>
#include "CONTROLLER.h"
#include "curva.h"
tool current_tool = T_PENCIL;
static int x1 = 0;
static int y1 = 0;
static int x2 = 0;
static int y2 = 0;
bool coor1 = false;
bool coor2 = false;
static void drawtools() {
  draw_sprite(&TOOL_PENCIL[0][0], TOOL_W, TOOL_H, 4, 4);
  draw_sprite(&TOOL_LINE[0][0], TOOL_W, TOOL_H, 4, 22);
  draw_sprite(&TOOL_CIRCLE[0][0], TOOL_W, TOOL_H, 4, 40);
  draw_sprite(&TOOL_SQUARE[0][0], TOOL_W, TOOL_H, 4, 57);
  draw_sprite(&TOOL_FILL[0][0], TOOL_W, TOOL_H, 4, 74);
  draw_sprite(&TOOL_ERASE[0][0], TOOL_W, TOOL_H, 4, 91);
  draw_sprite(&TOOL_SQ_ERASE[0][0], TOOL_W, TOOL_H, 4, 108);
}
int8_t checkcollision(void) {
  if (cursorx > 6 && cursorx < 14) {
    if (cursory > 4 && cursory < 4 + 8) {
      return 1;
    }
    if (cursory > 22 && cursory < 22 + 8) {
      return 2;
    }
    if (cursory > 40 && cursory < 40 + 8) {
      return 3;
    }
    if (cursory > 57 && cursory < 57 + 8) {
      return 4;
    }
    if (cursory > 74 && cursory < 74 + 8) {
      return 5;
    }
    if (cursory > 91 && cursory < 91 + 8) {
      return 6;
    }
    if (cursory > 108 && cursory < 108 + 8) {
      return 7;
    }
  }
  return -1;
}
void toolhandler() {
  if (cursorx > cxst && cursory < cyen) {
    switch (current_tool) {
      case T_PENCIL:
        if (a == LOW) {
          x1 = cursorx;
          y1 = cursory;
          set_pixel(x1, y1, 3);
        }
        break;
      case T_LINE:
        if (a == LOW) current_tool = T_PENCIL;
        break;
      case T_CIRCLE:
        if (a == LOW) {
          x1 = cursorx;
          y1 = cursory;
          coor1 = true;
        }
        if (x1 != cursorx || y1 != cursory) {
          x2 = cursorx;
          y2 = cursory;
          coor2 = true;
        }
        if ( coor1 == true && coor2 == true && a == HIGH && b == LOW) {
          int16_t radius = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
          circle(x1, y1, radius, 3);
          coor1 = false;
          coor2 = false;
        }
        break;
      case T_SQUARE:
        if (a == LOW) current_tool = T_PENCIL;
        break;
      case T_FILL:
        if (a == LOW) current_tool = T_PENCIL;
        break;
      case T_ERASE:
        if (a == LOW) current_tool = T_PENCIL;
        break;
      case T_SQ_ERASE:
        if (a == LOW) current_tool = T_PENCIL;
        break;
    }
  }
}
//PIXELS 160
//ROWS 120
void setuphandle() {
  drawbox(0, 0, PIXELS - 1, ROWS, 1, 3);
  drawbox(0, 0, 16, ROWS, 2, 3);
  drawbox(16, 112, PIXELS - 1, ROWS, 1, 3);
  drawtools();
}
int icon = -1;
void masterloop() {
  hide_cursor();
  if (u == LOW && cursory > 0) cursory--;
  if (d == LOW && cursory < ROWS - 3) cursory++;
  if (l == LOW && cursorx > 0) cursorx--;
  if (r == LOW && cursorx < PIXELS - 3) cursorx++;
  icon = checkcollision();
  if (icon != -1) {
    switch (icon) {
      case 1:
        if (a == LOW) current_tool = T_PENCIL;
        break;
      case 2:
        if (a == LOW) current_tool = T_LINE;
        break;
      case 3:
        if (a == LOW) current_tool = T_CIRCLE;
        break;
      case 4:
        if (a == LOW) current_tool = T_SQUARE;
        break;
      case 5:
        if (a == LOW) current_tool = T_FILL;
        break;
      case 6:
        if (a == LOW) current_tool = T_ERASE;
        break;
      case 7:
        if (a == LOW) current_tool = T_SQ_ERASE;
        break;
    }
  }
  toolhandler();
  update_cursor();
}