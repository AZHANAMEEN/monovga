#ifndef DRFX_H
#define DRFX_H
extern volatile int offset_x;
#include <stdint.h>

/* ---------------- screen settings ---------------- */
// If a specific game header hasn't defined these yet, use these defaults:
#define ROW_BYTES 40            // Default (e.g., Snake: 30 * 4 = 120 cols)
#define ROWS 120                // Default rows
#define PIXELS (ROW_BYTES * 4)  // Computed automatically



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
