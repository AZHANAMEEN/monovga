#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "font.h"
#include "drfx.h"

uint8_t fb[ROWS][ROW_BYTES];

/* ---------------- drawing functions ---------------- */

// Set one pixel (level 0-3) at any position. Off-screen is ignored.
void set_pixel(int16_t x, int16_t y, uint8_t level) {
  if (x < 0 || x >= PIXELS || y < 0 || y >= ROWS) return;
  uint8_t sh = (x & 3) << 1;
  uint8_t *p = &fb[y][x >> 2];
  *p = (*p & ~(3 << sh)) | ((level & 3) << sh);
}

void draw_sprite(const uint8_t *m, uint8_t w, uint8_t h, int16_t x, int16_t y) {
  for (uint8_t j = 0; j < h; j++) {
    int16_t py = y + j;
    if (py < 0 || py >= ROWS) continue;
    uint8_t *row = fb[py];
    for (uint8_t i = 0; i < w; i++) {
      int16_t px = x + i;
      if (px < 0 || px >= PIXELS) continue;

      // Calculate exact byte offset in flash for this pixel
      // m is a pointer to the start of the 2D array in flash
      uint8_t pixel = pgm_read_byte(m + (j * w) + i);

      if (pixel > 0) {  // Skip transparent/background pixels (level 0)
        uint8_t sh = (px & 3) << 1;
        uint8_t *p = &row[px >> 2];
        *p = (*p & ~(3 << sh)) | ((pixel & 3) << sh);
      }
    }
  }
}

// Fast black fill of a rectangle, used to erase a sprite before moving it.
// It works in whole bytes, so it can also blank up to 3 pixels beside the area.
void clear_rect(int16_t x, int16_t y, uint8_t w, uint8_t h) {
  int16_t x0 = x >> 2, x1 = (x + w - 1) >> 2;
  if (x0 < 0) x0 = 0;
  if (x1 > ROW_BYTES - 1) x1 = ROW_BYTES - 1;
  if (x0 > x1) return;
  for (uint8_t j = 0; j < h; j++) {
    int16_t py = y + j;
    if (py < 0 || py >= ROWS) continue;
    memset(&fb[py][x0], 0, x1 - x0 + 1);
  }
}

void clear_screen(void) {
  memset(fb, 0, sizeof fb);
}
void circle(int16_t cx, int16_t cy, int16_t r, uint8_t level) {
  int16_t x = r, y = 0, err = 1 - r;
  while (x >= y) {
    set_pixel(cx + x, cy + y, level);
    set_pixel(cx + y, cy + x, level);
    set_pixel(cx - y, cy + x, level);
    set_pixel(cx - x, cy + y, level);
    set_pixel(cx - x, cy - y, level);
    set_pixel(cx - y, cy - x, level);
    set_pixel(cx + y, cy - x, level);
    set_pixel(cx + x, cy - y, level);
    y++;
    if (err < 0) err += 2 * y + 1;
    else {
      x--;
      err += 2 * (y - x) + 1;
    }
  }
}
void drawbox(int start_x, int start_y, int width, int height, uint8_t thickness, uint8_t color) {
  for (int t = 0; t < thickness; t++) {
    for (int j = start_x; j < start_x + width; j++) {
      set_pixel(j, start_y + t, color);
      set_pixel(j, start_y + height - 1 - t, color);
    }
    for (int k = start_y; k < start_y + height; k++) {
      set_pixel(start_x + t, k, color);
      set_pixel(start_x + width - 1 - t, k, color);
    }
  }
}

//fontdrawer
void draw_char(int ind, int x, int y, uint8_t color) {
  if (ind < 0 || ind > 35) return;
  // Pass the pointer to the specific character matrix directly to your draw_sprite function
  // (Assuming draw_sprite is declared/available globally or via header)
  draw_sprite((const uint8_t *)&font5x8[ind], 5, 8, x, y);
}
void draw_char_literal(char c, int x, int y) {
  int ind = -1;
  if (c >= 'A' && c <= 'Z') ind = c - 'A';
  else if (c >= '0' && c <= '9') ind = (c - '0') + 26;

  draw_char(ind, x, y, 3);
}

/* ---------------- sprites ---------------- */

static const uint8_t smiley[8][8] PROGMEM = {
  { 0, 0, 3, 3, 3, 3, 0, 0 },
  { 0, 3, 3, 3, 3, 3, 3, 0 },
  { 3, 3, 0, 3, 3, 0, 3, 3 },
  { 3, 3, 3, 3, 3, 3, 3, 3 },
  { 3, 0, 3, 3, 3, 3, 0, 3 },
  { 3, 3, 0, 0, 0, 0, 3, 3 },
  { 0, 3, 3, 3, 3, 3, 3, 0 },
  { 0, 0, 3, 3, 3, 3, 0, 0 }
};
#define DVD_W 16
#define DVD_H 8

static const uint8_t dvd[DVD_H][DVD_W] PROGMEM = {
  { 3, 3, 3, 0, 0, 3, 0, 0, 0, 3, 0, 3, 3, 3, 0, 0 },
  { 3, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 3, 0 },
  { 3, 0, 0, 3, 0, 0, 3, 0, 3, 0, 0, 3, 0, 0, 3, 0 },
  { 3, 3, 3, 0, 0, 0, 0, 3, 0, 0, 0, 3, 3, 3, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0 },
  { 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3 },
  { 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0 }
};

/* ---------------- Animations ---------------- */
static int x = 30, y = 30, dx = 1, dy = 1, old_x = 30, old_y = 30;
void dvd_bounce(void) {
  clear_rect(old_x, old_y, 16, 8);
  x += dx;
  y += dy;
  if (x < 0) {
    x = 0;
    dx = -dx;
  } else if ((x + 16) > PIXELS) {
    dx = -dx;
    x = PIXELS - 16;
  }
  if (y < 0) {
    y = 0;
    dy = -dy;
  } else if ((y + 8) > ROWS) {
    dy = -dy;
    y = ROWS - 8;
  }
  draw_sprite(&dvd[0][0], 16, 8, x, y);
}
