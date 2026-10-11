# monovga

This is a simple program for the avr64da chips (though it should also work on other dx chips with some changes) for generating VGA video or static images it uses the event system of these chips to communicate between timers TCD0 and TCA0 for hsync and vsync. as default its loaded with a DVD bouncing animation.

# Progress

currently works on 640x480 res and locks on successfully, now color and pixels works too (160x120) for now.
<img width="1600" height="725" alt="WhatsApp Image 2026-10-10 at 1 35 45 PM" src="https://github.com/user-attachments/assets/03dd539d-d8b6-41c8-84c6-09c60e4bcd02" />
<img width="1600" height="900" alt="WhatsApp Image 2026-10-10 at 4 08 56 PM (1)" src="https://github.com/user-attachments/assets/b18a8857-1d1c-4b9d-ba12-abedd245e1e3" />
<img width="1600" height="900" alt="WhatsApp Image 2026-10-10 at 3 51 18 PM" src="https://github.com/user-attachments/assets/df88b651-d231-4f65-8cae-731c7f9fa633" />


# AVR64DA28 VGA output (160×120, 4 brightness levels)

Origin (0,0) is the top-left corner, x = 0-159 and y = 0-119. A level is 0 (black) to 3 (full brightness). Every drawing call clips to the screen, so off-screen positions are safe.

## Display and timing

| Function | What it does | Usage |
|---|---|---|
| `vsync_init()` | Starts TCA0 (VSYNC, counts lines through the event system). Call it before `hsync_init()`. | `vsync_init();` |
| `hsync_init()` | Sets the 24 MHz clock and starts TCD0 (HSYNC and the once-per-line interrupt). | `hsync_init();` |
| `wait_vblank()` | Sleeps the CPU until vertical blanking starts. | `wait_vblank();` |
| `delay_frames(n)` | Waits n frames (n × 16.7 ms) with the CPU asleep. Use it in place of `delay()`. | `delay_frames(30);` for about 0.5 s |
| `setup()` | Runs once before the display starts, with no time limit. Good for static backgrounds. | `circle(80, 60, 20, 3);` |
| `loop()` | Called once per frame (60 Hz) during vertical blanking. Keep it to about 1,000 pixel writes. | your animation code |

## Drawing

| Function | What it does | Usage |
|---|---|---|
| `set_pixel(x, y, level)` | Sets one pixel. | `set_pixel(80, 60, 3);` |
| `draw_sprite(m, w, h, x, y)` | Draws a w×h matrix of 0-3 values with its top-left at (x, y). Level 0 draws black. | `draw_sprite(&smiley[0][0], 8, 8, x, y);` |
| `clear_rect(x, y, w, h)` | Fast black fill, used to erase a sprite. It works in whole bytes, so it can also blank up to 3 pixels either side of the area. | `clear_rect(x, y, 16, 8);` |
| `clear_screen()` | Clears the whole framebuffer. | `clear_screen();` |
| `circle(cx, cy, r, level)` | Draws a circle outline. `r = 0` gives a dot. Level 0 erases a circle drawn earlier. | `circle(80, 60, 10, 3);` |

## Sprites

A sprite is `const uint8_t name[h][w]` with values 0-3. Included: `smiley` (8×8) and `dvd` (`DVD_W` × `DVD_H`, 16×8).

```c
draw_sprite(&dvd[0][0], DVD_W, DVD_H, x, y);
```

## Settings

| Name | Value | Meaning |
|---|---|---|
| `ROW_BYTES` | 40 | Bytes per row (4 pixels each) |
| `PIXELS` / `ROWS` | 160 / 120 | Screen size (each row is shown on 4 lines) |
| `BACKPORCH_DELAY` | 142 | Moves the picture sideways. One unit is one CPU cycle (about one screen pixel). |

# usage

you can simply open the test.ino file and upload it via a updi programmer or if you don't have one, you can easily find how to make one from a usb-ttl adapter , and just upload this code and connect your hsync pin to PA4 and vstync to PA0, it is reccomended to use a vga breakout board as direct wires can mess up your signals. 

Or if you want to use it on a Arduino you may need to code the hsync and vsync functions according to its timers and clock frequency.

PA0-VSYNC; PA4-HSYNC; PC0-MSB; PC1-LSB;

connect two resistors in series (I used 1.3k and ~680ohm) connect the lower ones end to lsb and the bigger ones end to msb and your r, g or b line to the junction between. and thats it, currently its monochrome either red black or blue black or green black or any fixed combination of these.
