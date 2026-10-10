#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <string.h>
#include "timings.h"          // VISIBLE_START_LINE, VISIBLE_END_LINE, ...
#include "sync_init.h"        // hsync_init(), vsync_init()
#include <math.h>

/* ---------------- screen settings ---------------- */
#define ROW_BYTES        40                  // 4 pixels per byte
#define PIXELS           (ROW_BYTES * 4)     // 160 columns
#define ROWS             120                 // each row is shown on 4 lines
#define BACKPORCH_DELAY  143                 // tune for horizontal position

static uint8_t fb[ROWS][ROW_BYTES];
volatile uint8_t vblank_start;               // set by the ISR once per frame

/* ---------------- pixel output (asm) ---------------- */
// 4 pixels per byte, 3 cycles per pixel, colour pins PC0 (LSB) and PC1 (MSB)
#define PIX4 \
    "ld  r16, X+\n out %[p], r16\n lsr r16\n lsr r16\n" \
    "out %[p], r16\n lsr r16\n lsr r16\n" \
    "out %[p], r16\n lsr r16\n lsr r16\n out %[p], r16\n"
#define PIX8    PIX4 PIX4
#define PIX40   PIX8 PIX8 PIX8 PIX8 PIX8        // 10 bytes
#define PIX160  PIX40 PIX40 PIX40 PIX40         // 40 bytes

static inline void burst(const uint8_t *row) {
    asm volatile(
        PIX160
        "out %[p], r1\n"                        // blank (r1 = 0 in avr-gcc)
        : "+x"(row)
        : [p] "I"(_SFR_IO_ADDR(VPORTC_OUT))
        : "r16");
}

ISR(TCD0_OVF_vect) {
    TCD0.INTFLAGS = TCD_OVF_bm;
    uint16_t line = TCA0.SINGLE.CNT;
    if (line < VISIBLE_START_LINE || line >= VISIBLE_END_LINE) {
        if (line == VISIBLE_END_LINE) vblank_start = 1;   // blanking begins
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

/* ---------------- drawing functions ---------------- */

// Set one pixel (level 0-3) at any position. Off-screen is ignored.
static void set_pixel(int16_t x, int16_t y, uint8_t level) {
    if (x < 0 || x >= PIXELS || y < 0 || y >= ROWS) return;
    uint8_t sh = (x & 3) << 1;
    uint8_t *p = &fb[y][x >> 2];
    *p = (*p & ~(3 << sh)) | ((level & 3) << sh);
}

// Draw a w x h matrix of 0-3 values (row by row) with its top-left at (x, y).
// Any x and y work, and parts outside the screen are clipped.
static void draw_sprite(const uint8_t *m, uint8_t w, uint8_t h, int16_t x, int16_t y) {
    for (uint8_t j = 0; j < h; j++, m += w) {
        int16_t py = y + j;
        if (py < 0 || py >= ROWS) continue;
        uint8_t *row = fb[py];
        for (uint8_t i = 0; i < w; i++) {
            int16_t px = x + i;
            if (px < 0 || px >= PIXELS) continue;
            uint8_t sh = (px & 3) << 1;
            uint8_t *p = &row[px >> 2];
            *p = (*p & ~(3 << sh)) | ((m[i] & 3) << sh);
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

static void clear_screen(void) { memset(fb, 0, sizeof fb); }

/* ---------------- your sprites ---------------- */
static const uint8_t smiley[8][8] = {
    {0,0,3,3,3,3,0,0},
    {0,3,3,3,3,3,3,0},
    {3,3,0,3,3,0,3,3},
    {3,3,3,3,3,3,3,3},
    {3,0,3,3,3,3,0,3},
    {3,3,0,0,0,0,3,3},
    {0,3,3,3,3,3,3,0},
    {0,0,3,3,3,3,0,0}
};

/* ---------------- your code ---------------- */

// Runs once, before the display starts.
void setup(void) {
    clear_screen();
    for (int a = 0; a < 360; a++)
    set_pixel(80 + (int16_t)(50 * cosf(a * 0.01745f)),
              60 + (int16_t)(40 * sinf(a * 0.01745f)), 3);
}

// Runs once per frame (60 times a second), inside vertical blanking.
// Keep it short: roughly 1000 pixel writes at most.
void loop(void) {
    static int16_t x = 0, y = 20, dx = 1, dy = 1;
    

    clear_rect(x, y, 8, 8);                          // erase old position
    if (x + dx < 0 || x + dx > PIXELS - 8) dx = -dx;
    if (y + dy < 0 || y + dy > ROWS - 8)   dy = -dy;
    x += dx;  y += dy;
    draw_sprite(&smiley[0][0], 8, 8, x, y);          // draw new position

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