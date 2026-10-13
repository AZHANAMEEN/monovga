#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <string.h>
#include <Arduino.h>
#include <stdlib.h>
#include "timings.h"    // VISIBLE_START_LINE, VISIBLE_END_LINE, ...
#include "sync_init.h"  // hsync_init(), vsync_init()
#include <math.h>
#include <avr/pgmspace.h>
#include "font.h"
#include "CONTROLLER.h"

/* ---------------- screen settings ---------------- */
#define ROW_BYTES 40            // 4 pixels per byte
#define PIXELS (ROW_BYTES * 4)  // 160 columns
#define ROWS 120                // each row is shown on 4 lines
#define BACKPORCH_DELAY 142     // tune for horizontal position

static uint8_t fb[ROWS][ROW_BYTES];
volatile uint8_t vblank_start;  // set by the ISR once per frame

/* ---------------- pixel output (asm) ---------------- */
// 4 pixels per byte, 3 cycles per pixel, colour pins PC0 (LSB) and PC1 (MSB)
#define PIX4 \
  "ld  r16, X+\n out %[p], r16\n lsr r16\n lsr r16\n" \
  "out %[p], r16\n lsr r16\n lsr r16\n" \
  "out %[p], r16\n lsr r16\n lsr r16\n out %[p], r16\n"
#define PIX8 PIX4 PIX4
#define PIX40 PIX8 PIX8 PIX8 PIX8 PIX8  // 10 bytes
#define PIX160 PIX40 PIX40 PIX40 PIX40  // 40 bytes

static inline void burst(const uint8_t *row) {
  asm volatile(
    PIX160
    "out %[p], r1\n"  // blank (r1 = 0 in avr-gcc)
    : "+x"(row)
    : [p] "I"(_SFR_IO_ADDR(VPORTC_OUT))
    : "r16");
}

ISR(TCD0_OVF_vect) {
  TCD0.INTFLAGS = TCD_OVF_bm;
  uint16_t line = TCA0.SINGLE.CNT;
  if (line < VISIBLE_START_LINE || line >= VISIBLE_END_LINE) {
    if (line == VISIBLE_END_LINE) vblank_start = 1;  // blanking begins
    return;
  }
  const uint8_t *row = fb[(line - VISIBLE_START_LINE) >> 2];
  __builtin_avr_delay_cycles(BACKPORCH_DELAY);
  burst(row);
}

static void wait_vblank(void) {
  vblank_start = 0;
  while (!vblank_start) sleep_mode();
}
static void delay_frames(uint8_t n) {
  while (n--) wait_vblank();
}
/* ---------------- drawing functions ---------------- */

// Set one pixel (level 0-3) at any position. Off-screen is ignored.
static void set_pixel(int16_t x, int16_t y, uint8_t level) {
  if (x < 0 || x >= PIXELS || y < 0 || y >= ROWS) return;
  uint8_t sh = (x & 3) << 1;
  uint8_t *p = &fb[y][x >> 2];
  *p = (*p & ~(3 << sh)) | ((level & 3) << sh);
}

static void draw_sprite(const uint8_t *m, uint8_t w, uint8_t h, int16_t x, int16_t y) {
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
static void clear_rect(int16_t x, int16_t y, uint8_t w, uint8_t h) {
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

static void clear_screen(void) {
  memset(fb, 0, sizeof fb);
}
static void circle(int16_t cx, int16_t cy, int16_t r, uint8_t level) {
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
/* ---------------- your sprites ---------------- */


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
#define snekh 6
#define snekw 6
static const uint8_t snek[snekh][snekw] PROGMEM = {
  { 0, 3, 3, 3, 3, 0 },
  { 3, 2, 2, 2, 2, 3 },
  { 3, 0, 2, 2, 0, 3 },  // eyes
  { 3, 2, 2, 2, 2, 3 },
  { 3, 2, 2, 2, 2, 3 },
  { 0, 3, 3, 3, 3, 0 }
};
static const uint8_t body[snekh][snekw] PROGMEM = {
  { 0, 3, 3, 3, 3, 0 },
  { 3, 2, 2, 2, 2, 3 },
  { 3, 2, 2, 2, 2, 3 },
  { 3, 2, 2, 2, 2, 3 },
  { 3, 2, 2, 2, 2, 3 },
  { 0, 3, 3, 3, 3, 0 }
};
#define applew 6
#define appleh 6
static const uint8_t apple[appleh][applew] PROGMEM = {
  { 0, 0, 3, 0, 0, 0 },
  { 0, 3, 2, 2, 3, 0 },
  { 3, 2, 1, 1, 2, 3 },
  { 3, 1, 1, 1, 1, 3 },
  { 3, 2, 1, 1, 2, 3 },
  { 0, 3, 2, 2, 3, 0 }
};
/* ---------------- Animations ---------------- */
static int x = 30, y = 30, dx = 1, dy = 1, old_x = 30, old_y = 30;
static void dvd_bounce(void) {
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
/*--------------game-------------*/
#define MAX_SNAKE_LEN 100
int px[MAX_SNAKE_LEN];
int py[MAX_SNAKE_LEN];
int snake_len = 3;  // Start with a length of 3
static int dirx, diry;
int fx;
int fy;
int fruit = 0;
static void fruitspawn(void) {
  if (fruit == 0) {
    bool collision;
    do {
      collision = false;
      fx = 6 + (rand() % 12) * 6;
      fy = 6 + (rand() % 12) * 6;
      
      // Check if apple spawned on any part of the snake
      for (int i = 0; i < snake_len; i++) {
        if (fx == px[i] && fy == py[i]) {
          collision = true;
          break;
        }
      }
    } while (collision);

    fruit = 1;
  }
  
  // Draw the apple every frame so it doesn't get cleared by background refreshes
  draw_sprite(&apple[0][0], 6, 6, fx, fy);
}
static void sneksetup(void) {
  //mainborder
  drawbox(0, 0, 159, 120, 4, 3);
  //gameborder
  drawbox(0, 0, 86, 86, 4, 3);
  //score screen
  draw_char_literal('S', 107, 12);
  draw_char_literal('C', 112 + 1, 12);
  draw_char_literal('O', 117 + 2, 12);
  draw_char_literal('R', 122 + 3, 12);
  draw_char_literal('E', 127 + 4, 12);
  draw_char_literal('M', 12, 100);
  draw_char_literal('O', 18, 100);
  draw_char_literal('N', 24, 100);
  draw_char_literal('O', 30, 100);
  draw_char_literal('V', 36, 100);
  draw_char_literal('G', 42, 100);
  draw_char_literal('A', 48, 100);
  for (int i = 0; i < snake_len; i++) {  // head on the right, body trailing behind it
    px[i] = 6 + 6 * (snake_len - 1 - i);
    py[i] = 6;
  };
}
void snakeinit(void) {
  dirx = 1;
  diry = 0;
  snake_len = 3;
  //px[0] = 6;
  //py[0] = 6;
}
int t = 0;
int score = 0;  // or whatever your score formula is
void snekloop() {
  t += 1;

  // Check if head is within bounds
  if ((px[0] > 3 && px[0] < 85) && (py[0] > 3 && py[0] < 85)) {

    if (t % 10 == 0) {
      // 1. Clear ONLY the old tail position before shifting
      clear_rect(px[snake_len - 1], py[snake_len - 1], snekw, snekh);

      // 2. Shift body segments from tail to head
      for (int i = snake_len - 1; i > 0; i--) {
        px[i] = px[i - 1];
        py[i] = py[i - 1];
      }

      // 3. Step the head forward
      px[0] += 6 * dirx;
      py[0] += 6 * diry;
    }
    // 4. Draw every frame: head + body segments
    draw_sprite(&snek[0][0], snekw, snekh, px[0], py[0]);

    for (int i = 1; i < snake_len; i++) {
      draw_sprite(&body[0][0], snekw, snekh, px[i], py[i]);
    }
    //char d2 = '0' + ((score / 100) % 10);
    //char d1 = '0' + ((score / 10) % 10);
    //char d0 = '0' + (score % 10);
    //if (!(d2 == '0')) clear_rect(107, 25, 5, 8);
    //if (!(d1 == '0')) clear_rect(112 + 6, 25, 5, 8);
    //if (!(d0 == '0')) clear_rect(117 + 12, 25, 5, 8);
    //draw_char_literal(d2, 107, 25);
    //draw_char_literal(d1, 112 + 6, 25);
    //draw_char_literal(d0, 117 + 12, 25);
    
    // If head hits the apple
    if (fx == px[0] && fy == py[0]) {
      fruit = 0;
      score += 1;
      if (snake_len < 30) {
        snake_len++;  // Grow the snake
      }
    }
  }
  fruitspawn();
  // Boundary check
  if (!(px[0] > 3) || !(px[0] + 6 < 85) || !(py[0] > 3) || !(py[0] + 6 < 85)) {
    clear_screen();
    sneksetup();
    snakeinit();
  }
  if (u == LOW && diry != 1) {
    dirx = 0;
    diry = -1;  // Assuming -1 is Up
  }
  if (d == LOW && diry != -1) {
    dirx = 0;
    diry = 1;  // Assuming 1 is Down
  }
  if (l == LOW && dirx != 1) {
    dirx = -1;
    diry = 0;  // Assuming -1 is Left
  }
  if (r == LOW && dirx != -1) {
    dirx = 1;
    diry = 0;  // Assuming 1 is Right
  }
  
}


/* ---------------- your code ---------------- */
// Runs once, before the display starts.
void setup(void) {
  clear_screen();
  sneksetup();
  snakeinit();
  srand(TCB0.CNT);
  setupcont();
}

// Runs once per frame (60 times a second), inside vertical blanking.
// Keep it short: roughly 1000 pixel writes at most.
void loop(void) {
  loopcont();
  snekloop();
  delay_frames(2);
}
/* ---------------- startup ---------------- */
int main(void) {
  PORTC.DIRSET = PIN0_bm | PIN1_bm;
  setup();
  vsync_init();
  hsync_init();
  set_sleep_mode(SLEEP_MODE_IDLE);
  sei();
  for (;;) {
    wait_vblank();
    loop();
  }
}