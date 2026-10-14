#ifndef DRFX_H
#define DRFX_H

#include <stdint.h>

/* ---------------- screen settings ---------------- */
#define ROW_BYTES 25            // 4 pixels per byte
#define PIXELS (ROW_BYTES * 4)  // 160 columns
#define ROWS 120                // each row is shown on 4 lines

extern uint8_t fb[ROWS][ROW_BYTES];

/* ---------------- drawing functions ---------------- */
void set_pixel(int16_t x, int16_t y, uint8_t level);
void draw_sprite(const uint8_t *m, uint8_t w, uint8_t h, int16_t x, int16_t y);
void clear_rect(int16_t x, int16_t y, uint8_t w, uint8_t h);
void clear_screen(void);
void circle(int16_t cx, int16_t cy, int16_t r, uint8_t level);
void drawbox(int start_x, int start_y, int width, int height, uint8_t thickness, uint8_t color);
void draw_char(int ind, int x, int y, uint8_t color);
void draw_char_literal(char c, int x, int y);

/* ---------------- animations ---------------- */
void dvd_bounce(void);

#endif
