#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "drfx.h"
#include "snake.h"
#include "CONTROLLER.h"

/* ---------------- your sprites ---------------- */
#define snekh 4
#define snekw 4
static const uint8_t snek[snekh][snekw] PROGMEM = {
  { 3, 3, 3, 3 },
  { 3, 2, 0, 3 },  // eyes
  { 3, 2, 0, 3 },
  { 3, 3, 3, 3 }
};

#define bodyh 4
#define bodyw 4
static const uint8_t body[bodyh][bodyw] PROGMEM = {
  { 3, 3, 3, 3 },
  { 3, 2, 2, 3 },  // eyes
  { 3, 2, 2, 3 },
  { 3, 3, 3, 3 }
};

#define applew 4
#define appleh 4
static const uint8_t apple[appleh][applew] PROGMEM = {
  { 3, 3, 3, 3 },
  { 3, 2, 1, 3 },
  { 3, 1, 2, 3 },
  { 3, 3, 3, 3 }
};
/*--------------game-------------*/
#define MAX_SNAKE_LEN 30
int px[MAX_SNAKE_LEN];
int py[MAX_SNAKE_LEN];
int snake_len = 3;  // Start with a length of 3
static int dirx, diry;
int fx;
int fy;
int t = 0;
int score = 0;  // or whatever your score formula is
int fruit = 0;
bool w=1,o=1,s=1,p=1;
void fruitspawn(void) {
  if (fruit == 0) {
    bool collision;
    do {
      collision = false;
      fx = 4 + (rand() % 20) * 4;
      fy = 16 + (rand() % 20) * 4;

      // Check if apple spawned on any part of the snake
      for (int i = 0; i < snake_len; i++) {
        if (fx == px[i] && fy == py[i]) {
          collision = true;
          break;
        }
      }
    } while (collision);

    fruit = 1;
    draw_sprite(&apple[0][0], applew, appleh, fx, fy);  // drawn once, at spawn
  }
}
void sneksetup(void) {
  //mainborder
  drawbox(0, 0, 99, 120, 2, 3);
  //gameborder
  drawbox(2, 14, 95, 84, 2, 3);
  //score screen
  draw_char_literal('M', 4, 4);
  draw_char_literal('O', 9 + 1, 4);
  draw_char_literal('N', 14 + 2, 4);
  draw_char_literal('O', 19 + 3, 4);
  draw_char_literal('V', 24 + 4, 4);
  draw_char_literal('G', 29 + 5, 4);
  draw_char_literal('A', 34 + 6, 4);
}
void snakeinit(void) {
    dirx = 1;
    diry = 0;
    snake_len = 3;
    score = 0;
    fruit = 0;
    t = 0;

    px[0] = 16;
    py[0] = 36;

    px[1] = 12;
    py[1] = 36;

    px[2] = 8;
    py[2] = 36;
}

void snekloop(void) {
  t += 1;
  // Check if head is within bounds
  if (t % 10 == 0) {
    if (u == LOW && diry != 1) {
      dirx = 0;
      diry = -1;
    } else if (d == LOW && diry != -1) {
      dirx = 0;
      diry = 1;
    } else if (l == LOW && dirx != 1) {
      dirx = -1;
      diry = 0;
    } else if (r == LOW && dirx != -1) {
      dirx = 1;
      diry = 0;
    }
    // 1. Clear ONLY the old tail position before shifting
    clear_rect(px[snake_len - 1], py[snake_len - 1], bodyw, bodyh);
    // 2. Shift body segments from tail to head
    for (int i = snake_len - 1; i > 0; i--) {
      px[i] = px[i - 1];
      py[i] = py[i - 1];
    }
    // 3. Step the head forward by 4 pixels
    px[0] += 4 * dirx;
    py[0] += 4 * diry;
  }

  // 4. Draw every frame: head + body segments
  draw_sprite(&snek[0][0], snekw, snekh, px[0], py[0]);

  for (int i = 1; i < snake_len; i++) {
    draw_sprite(&body[0][0], bodyw, bodyh, px[i], py[i]);
  }


  // If head hits the apple
  if (fx == px[0] && fy == py[0]) {
    clear_rect(fx,fy,applew,appleh);
    fruit = 0;
    score += 1;
    if (snake_len < 30) {
      //snake_len++;  // Grow the snake
    }
  }


  fruitspawn();
  // Boundary check
  if ((px[0] < 4) || ((px[0] + snekw) > 94) || (py[0] < 16) || ((py[0] + snekh) > 94)) {
    clear_screen();
    sneksetup();
    snakeinit();
  }
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