#include "applicationhandler.h"
#include "snake.h"
#include "drfx.h"
#include <avr/pgmspace.h>
#include "CONTROLLER.h"
#include "curva.h"
ScreenState current_state = SCREEN_HOME;
/*CURSOR SPRITE*/
#define iconW 9
#define iconH 8
const uint8_t CURSOR[3][3] PROGMEM = {

  { 0, 3, 0 },
  { 3, 3, 3 },
  { 0, 3, 0 }

};
static const uint8_t tetris[iconH][iconW] PROGMEM = {
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 0, 0, 3, 3, 3, 3, 0, 3 },
  { 3, 0, 0, 0, 0, 0, 3, 0, 3 },
  { 3, 0, 3, 0, 0, 0, 0, 0, 3 },
  { 3, 0, 3, 3, 3, 3, 3, 0, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 }
};
static const uint8_t snek[iconH][iconW] PROGMEM = {
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 0, 3, 3, 3, 3, 3, 0, 3 },
  { 3, 0, 3, 0, 0, 0, 3, 0, 3 },
  { 3, 0, 3, 0, 3, 0, 3, 0, 3 },
  { 3, 0, 3, 0, 3, 3, 3, 0, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 }
};
static const uint8_t curva[iconH][iconW] PROGMEM = {
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 0, 0, 3, 0, 3, 0, 0, 3 },
  { 3, 0, 3, 0, 3, 0, 3, 0, 3 },
  { 3, 0, 0, 3, 0, 3, 0, 0, 3 },
  { 3, 0, 3, 0, 0, 0, 3, 0, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 }
};
static const uint8_t SETINGS[iconH][iconW] PROGMEM = {
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 0, 3, 3, 3, 0, 3, 0, 3 },
  { 3, 0, 3, 0, 3, 0, 3, 0, 3 },
  { 3, 0, 3, 0, 3, 0, 3, 0, 3 },
  { 3, 0, 3, 0, 3, 3, 3, 0, 3 },
  { 3, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 3, 3, 3, 3, 3, 3, 3, 3, 3 }
};
static int of = 110;
volatile int offset_x = 0;
void taskbar() {
  drawbox(0, ROWS - 10, PIXELS - 1, ROWS, 1, 3);
}
// Define your icon dimensions (adjust if they are 8x8 or 12x12, etc.)
#define ICON_W 8
#define ICON_H 8

// Function to check which icon the cursor is hovering over (returns index 0-3, or -1 if none)
int8_t checkIconCollision(int16_t cx, int16_t cy) {
  // X is always 4 for all your icons
  int16_t icon_x = 4;

  // Y coordinates calculated matching your placement logic
  int16_t icon_y[4] = {
    4 + 6,                                // SETTINGS
    4 + ICON_H + 1 + 6,                   // SNEAK
    4 + ICON_H + ICON_H + 2 + 6,          // TETRIS
    4 + ICON_H + ICON_H + ICON_H + 3 + 6  // CURVA
  };

  // Loop through all 4 icons to check bounding box collision
  for (uint8_t i = 0; i < 4; i++) {
    // Check if cursor bounding box overlaps with the icon bounding box
    // (Assuming cursor is 5x7 based on your CURSOR array)
    if (cx + 5 > icon_x && cx < icon_x + ICON_W && cy + 7 > icon_y[i] && cy < icon_y[i] + ICON_H) {
      return i;  // Returns 0=Settings, 1=Snake, 2=Tetris, 3=Curva
    }
  }

  return -1;  // Not hovering over any icon
}

void uidrawroutine() {
  drawbox(0, 0, PIXELS - 1, ROWS, 2, 3);
  // HEADER: MONOVGA
  draw_char_literal('M', 4 + of, 4);
  draw_char_literal('O', 9 + 1 + of, 4);
  draw_char_literal('N', 14 + 2 + of, 4);
  draw_char_literal('O', 19 + 3 + of, 4);
  draw_char_literal('V', 24 + 4 + of, 4);
  draw_char_literal('G', 29 + 5 + of, 4);
  draw_char_literal('A', 34 + 6 + of, 4);
  //ICONS (BAD MWETHDO IKIK)
  draw_sprite(&SETINGS[0][0], iconW, iconH, 4, 4 + 6);
  draw_sprite(&snek[0][0], iconW, iconH, 4, 4 + iconH + 1 + 6);
  draw_sprite(&tetris[0][0], iconW, iconH, 4, 4 + iconH + iconH + 1 + 1 + 6);
  draw_sprite(&curva[0][0], iconW, iconH, 4, 4 + iconH + iconH + iconH + 1 + 1 + 1 + 6);
}

void homesetup() {
  // UI Layout / Header rendering
  uidrawroutine();
  cursorx = PIXELS / 2;
  cursory = ROWS / 2;
}
extern int cursorx = 0;
extern int cursory = 0;
static int ox = 0;
static int oy = 0;
static int dx = 1;
static int last_state = -1;  // -1 = nothing run yet, so the first frame runs the setup

void stateTick(void) {  // call this once per frame from loop()
  if (current_state != last_state /*|| current_state == SCREEN_HOME)*/) {
    switchsetup();               // runs once per change; the default case may reset current_state
    last_state = current_state;  // set AFTER the setup so that change doesn't re-trigger it
  }
}
#define CURSOR_W 3
#define CURSOR_H 3

static uint8_t cursor_bg[CURSOR_H][CURSOR_W];
static int16_t old_x = -1;
static int16_t old_y = -1;

static uint8_t get_pixel(int16_t x, int16_t y)
{
    uint8_t packed = fb[y][x >> 2];
    return (packed >> ((x & 3) * 2)) & 3;
}

static void put_pixel(int16_t x, int16_t y, uint8_t color)
{
    uint8_t shift = (x & 3) * 2;
    uint8_t *p = &fb[y][x >> 2];

    *p = (*p & ~(3u << shift)) | ((color & 3u) << shift);
}

static void save_cursor_bg(int16_t x, int16_t y)
{
    for (uint8_t j = 0; j < CURSOR_H; j++)
        for (uint8_t i = 0; i < CURSOR_W; i++)
            cursor_bg[j][i] = get_pixel(x + i, y + j);
}

static void restore_cursor_bg(void)
{
    if (old_x < 0) return;

    for (uint8_t j = 0; j < CURSOR_H; j++)
        for (uint8_t i = 0; i < CURSOR_W; i++)
            put_pixel(old_x + i, old_y + j, cursor_bg[j][i]);
}

void update_cursor(void)
{
    if (cursorx == old_x && cursory == old_y)
        return;

    restore_cursor_bg();

    save_cursor_bg(cursorx, cursory);
    draw_sprite(&CURSOR[0][0], CURSOR_W, CURSOR_H,
                cursorx, cursory);

    old_x = cursorx;
    old_y = cursory;
}

void invalidate_cursor(void)
{
  old_x = -1;
  old_y = -1;
}

void hide_cursor(void)
{
  restore_cursor_bg();
  invalidate_cursor();
}

const int start_x = 10;
const int ya = 110;
void draw_xval(void) {              // called only when the score changes
  clear_rect(start_x, ya, 3, 5);  drawsm_char_literal('0' + (cursorx / 100) % 10, start_x, ya);
  clear_rect(start_x, ya, 3, 5);  drawsm_char_literal('0' + (cursorx / 10) % 10, start_x+5+1, ya);
  clear_rect(start_x, ya, 3, 5);  drawsm_char_literal('0' + cursorx % 10, start_x+5+5+1+1, ya);
}
// Since numbers start at index 26 ('0' is 26, '1' is 27, etc.)
// or you can pass them as characters using '0' + digit:

void homeloop() {
  taskbar();
  int8_t hovered_app = checkIconCollision(cursorx, cursory);
  if (u == LOW) {
    if (cursory > 11) cursory -= dx;update_cursor();
  }
  if (d == LOW) {
    if (cursory < (ROWS - 8)) cursory += dx;update_cursor();
  }
  if (l == LOW) {
    if (cursorx > 4) cursorx -= dx;update_cursor();
  }
  if (r == LOW) {
    if (cursorx < (PIXELS - 1 - 8)) cursorx += dx;update_cursor();
  }

  if (hovered_app != -1) {
    switch (hovered_app) {
      case 0:
        if (a == LOW) current_state = SCREEN_SETTINGS;
        break;
      case 1:
        if (a == LOW) current_state = SCREEN_SNAKE;
        break;
      case 2:
        if (a == LOW) current_state = SCREEN_TETRIS;
        break;
      case 3:
        if (a == LOW) current_state = SCREEN_CURVA;
        break;
    }
  }
}
void switchsetup() {
  clear_screen();
  switch (current_state) {
    case SCREEN_HOME:
      offset_x = 0;
      homesetup();
      break;
    case SCREEN_SNAKE:
      offset_x = 30;
      sneksetup();
      break;
    case SCREEN_TETRIS:
      
      break;
    case SCREEN_SETTINGS:
      // Run settings menu
      break;
    case SCREEN_CURVA:
      offset_x = 0;
      invalidate_cursor();
      setuphandle();
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
      if (x == LOW) current_state = SCREEN_HOME;
      break;
    case SCREEN_TETRIS:
      if (x == LOW) current_state = SCREEN_HOME;
      break;
    case SCREEN_SETTINGS:
      if (x == LOW) current_state = SCREEN_HOME;
      break;
    case SCREEN_CURVA:
      masterloop();
      if (x == LOW) current_state = SCREEN_HOME;
      break;
  }
  //stateTick();
}