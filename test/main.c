#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "timings.h"
#include "sync_init.h"

ISR(TCD0_OVF_vect) {
    TCD0.INTFLAGS = TCD_OVF_bm;     // clear flag
    // pixel output goes here later
}

int main(void) {
    vsync_init();
    hsync_init();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
    for (;;) sleep_mode();
}