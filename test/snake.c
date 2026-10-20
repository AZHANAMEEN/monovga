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
int snake_len = 6;  // Start with a length of 3
static int dirx, diry;
int fx;
int fy;
int t = 0;
int score = 0;  // or whatever your score formula is
int fruit = 0;
bool w = 1, o = 1, s = 1, p = 1;
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
  offset_x = 30;
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
  snakeinit();
}
static int want_x = 1, want_y = 0;

static void draw_score(void) {              // called only when the score changes
  clear_rect(21, 100, 5, 8);  draw_char_literal('0' + (score / 100) % 10, 22, 100);
  clear_rect(50, 100, 5, 8);  draw_char_literal('0' + (score / 10) % 10, 50, 100);
  clear_rect(78, 100, 5, 8);  draw_char_literal('0' + score % 10, 78, 100);
}
static bool hit_self(void) {
  for (int i = 1; i < snake_len; i++)
    if (px[0] == px[i] && py[0] == py[i]) return true;
  return false;
}
void snakeinit(void) {
  dirx = want_x = 1;
  diry = want_y = 0;
  snake_len = 12;
  score = 0;
  fruit = 0;
  t = 0;
  for (int i = 0; i < snake_len; i++) {      // head at x=48, body trailing left to x=4
    px[i] = 48 - 4 * i;
    py[i] = 36;
  }
  draw_sprite(&snek[0][0], snekw, snekh, px[0], py[0]);
  for (int i = 1; i < snake_len; i++)
    draw_sprite(&body[0][0], bodyw, bodyh, px[i], py[i]);
  draw_score();
  fruitspawn();
}

void snekloop(void) {
  if      (u == LOW && diry != 1)  { want_x = 0;  want_y = -1; }   // read the pad every frame
  else if (d == LOW && diry != -1) { want_x = 0;  want_y = 1;  }
  else if (l == LOW && dirx != 1)  { want_x = -1; want_y = 0;  }
  else if (r == LOW && dirx != -1) { want_x = 1;  want_y = 0;  }

  if (++t < 10) return;                      // 9 frames in 10 do nothing
  t = 0;
  dirx = want_x;  diry = want_y;

  int tx = px[snake_len - 1], ty = py[snake_len - 1];       // old tail cell
  draw_sprite(&body[0][0], bodyw, bodyh, px[0], py[0]);     // old head becomes body

  for (int i = snake_len - 1; i > 0; i--) { px[i] = px[i - 1]; py[i] = py[i - 1]; }
  px[0] += 4 * dirx;
  py[0] += 4 * diry;

  if (px[0] < 4 || px[0] + snekw > 94 || py[0] < 16 || py[0] + snekh > 96 || hit_self()) {   // wall
    clear_screen();
    sneksetup();
    snakeinit();
    return;
  }

  if (px[0] == fx && py[0] == fy) {          // ate the apple
    clear_rect(fx, fy, applew, appleh);
    fruit = 0;
    score++;
    if (snake_len < MAX_SNAKE_LEN) {         // grow: the old tail cell stays occupied
      snake_len++;
      px[snake_len - 1] = tx;
      py[snake_len - 1] = ty;
    } else {
      clear_rect(tx, ty, bodyw, bodyh);
    }
    draw_score();
  } else {
    clear_rect(tx, ty, bodyw, bodyh);        // normal move: erase the old tail
  }

  draw_sprite(&snek[0][0], snekw, snekh, px[0], py[0]);     // new head
  fruitspawn();                                             // draws only if a new apple is needed
}