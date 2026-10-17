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
#include "CONTROLLER.h"
#include "drfx.h"   // fb, ROW_BYTES, PIXELS, ROWS, drawing functions
#include "snake.h"  // sneksetup(), snekloop()
#include "applicationhandler.h"
/* ---------------- screen settings ---------------- */
#define BACKPORCH_DELAY 142     // tune for horizontal position

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
#define PIX100 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 //100BYTES
#define PIX120 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 PIX4 //120BYTES
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
/* ---------------- your code ---------------- */

// Runs once, before the display starts.
void setup(void) {
  clear_screen();
  setupcont();
  switchsetup();
}
// Runs once per frame (60 times a second), inside vertical blanking.
// Keep it short: roughly 1000 pixel writes at most.
void loop(void) {
  loopcont();
  updateScreen();
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
