#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "font.h"
#include "drfx.h"

uint8_t fb[ROWS][ROW_BYTES];

/* ---------------- drawing functions ---------------- */

// Set one pixel (level 0-3) at any position. Off-screen is ignored.
void set_pixel(int16_t u, int16_t y, uint8_t level) {
  int x = u + offset_x;
  if (x < 0 || x >= PIXELS || y < 0 || y >= ROWS) return;
  uint8_t sh = (x & 3) << 1;
  uint8_t *p = &fb[y][x >> 2];
  *p = (*p & ~(3 << sh)) | ((level & 3) << sh);
}

void draw_sprite(const uint8_t *m, uint8_t w, uint8_t h, int16_t u, int16_t y) {
  int x = u + offset_x;
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

// Pixel-exact filled rectangle, any level (0 = erase). Honours offset_x like the other draw functions.
void fill_rect(int16_t u, int16_t y, uint8_t w, uint8_t h, uint8_t level) {
  int16_t x  = u + offset_x;
  int16_t x1 = x + w, y1 = y + h;                  // exclusive ends
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x1 > PIXELS) x1 = PIXELS;
  if (y1 > ROWS)   y1 = ROWS;
  if (x >= x1 || y >= y1) return;

  uint8_t pat = (level & 3) * 0x55;
  uint8_t xb0 = x >> 2, xb1 = (x1 - 1) >> 2;
  uint8_t lm  = 0xFF << ((x & 3) << 1);
  uint8_t rm  = 0xFF >> ((3 - ((x1 - 1) & 3)) << 1);

  for (int16_t j = y; j < y1; j++) {
    uint8_t *row = fb[j];
    if (xb0 == xb1) {
      uint8_t m = lm & rm;
      row[xb0] = (row[xb0] & ~m) | (pat & m);
    } else {
      row[xb0] = (row[xb0] & ~lm) | (pat & lm);
      if (xb1 - xb0 > 1) memset(&row[xb0 + 1], pat, xb1 - xb0 - 1);
      row[xb1] = (row[xb1] & ~rm) | (pat & rm);
    }
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
void drawsm_char(int ind, int x, int y, uint8_t color) {
  if (ind < 0 || ind > 35) return;
  // Pass the pointer to the specific character matrix directly to your draw_sprite function
  // (Assuming draw_sprite is declared/available globally or via header)
  draw_sprite((const uint8_t *)&font3x5[ind], 5, 8, x, y);
}
void drawsm_char_literal(char c, int x, int y) {
  int ind = -1;
  if (c >= 'A' && c <= 'Z') ind = c - 'A';
  else if (c >= '0' && c <= '9') ind = (c - '0') + 26;
  drawsm_char(ind, x, y, 3);
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
