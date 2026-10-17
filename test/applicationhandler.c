#include "applicationhandler.h"
#include "snake.h"
#include "drfx.h"
/*CURSOR SPRITE*/

static int of = 35;
volatile int offset_x = 0;
void homesetup() {
  // UI Layout / Header rendering
  drawbox(0, 0, 120, ROWS - 1, 2, 3);
  // HEADER: MONOVGA
  draw_char_literal('M', 0 + 4 + of, 4);
  draw_char_literal('O', 0 + 9 + 1 + of, 4);
  draw_char_literal('N', 0 + 14 + 2 + of, 4);
  draw_char_literal('O', 0 + 19 + 3 + of, 4);
  draw_char_literal('V', 0 + 24 + 4 + of, 4);
  draw_char_literal('G', 0 + 29 + 5 + of, 4);
  draw_char_literal('A', 0 + 34 + 6 + of, 4);
  //MENU ITEMS (BAD MWETHDO IKIK)
  //SNAKE
  draw_char_literal('S', 45 + 0, 33);
  draw_char_literal('N', 45 + 6, 33);
  draw_char_literal('A', 45 + 12, 33);
  draw_char_literal('K', 45 + 18, 33);
  draw_char_literal('E', 45 + 24, 33);
  //TETRIS
  draw_char_literal('T', 42 + 0, 54);
  draw_char_literal('E', 42 + 6, 54);
  draw_char_literal('T', 42 + 12, 54);
  draw_char_literal('R', 42 + 18, 54);
  draw_char_literal('I', 42 + 24, 54);
  draw_char_literal('S', 42 + 30, 54);
  //CURVA
  draw_char_literal('C', 45 + 0, 75);
  draw_char_literal('U', 45 + 6, 75);
  draw_char_literal('R', 45 + 12, 75);
  draw_char_literal('V', 45 + 18, 75);
  draw_char_literal('A', 45 + 24, 75);
  //SETTINGS
  draw_char_literal('S', 36 + 0, 96);
  draw_char_literal('E', 36 + 6, 96);
  draw_char_literal('T', 36 + 12, 96);
  draw_char_literal('T', 36 + 18, 96);
  draw_char_literal('I', 36 + 24, 96);
  draw_char_literal('N', 36 + 30, 96);
  draw_char_literal('G', 36 + 36, 96);
  draw_char_literal('S', 36 + 42, 96);
}
void homeloop() {
}
// Set default state to home
ScreenState current_state = SCREEN_HOME;

void switchsetup() {
  clear_screen();
  switch (current_state) {
    case SCREEN_HOME:
      offset_x = 20;
      homesetup();
      break;
    case SCREEN_SNAKE:
      offset_x = 30;
      sneksetup();
      break;
    case SCREEN_TETRIS:
      // Run tetris color-plane logic
      break;
    case SCREEN_SETTINGS:
      // Run settings menu
      break;
    case SCREEN_MVPAINT:
      // Run MV-Paint vector canvas suite
      break;
    default:
      offset_x = 20;
      current_state = SCREEN_HOME;
      homesetup();
  }
}
void updateScreen() {
  switch (current_state) {
    case SCREEN_HOME:
      homeloop();
      break;
    case SCREEN_SNAKE:
      snekloop();
      break;
    case SCREEN_TETRIS:
      // Run tetris color-plane logic
      break;
    case SCREEN_SETTINGS:
      // Run settings menu
      break;
    case SCREEN_MVPAINT:
      // Run MV-Paint vector canvas suite
      break;
    default:
      homeloop();
  }
}