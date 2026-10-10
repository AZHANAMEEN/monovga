#include <avr/io.h>
#include <avr/cpufunc.h>
#include "timings.h"
#include "sync_init.h"

void hsync_init(void) {
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL_FRQSEL_24M_gc);
    PORTA.DIRSET   = PIN4_bm;
    PORTA.PIN4CTRL = PORT_INVEN_bm;

    TCD0.CTRLB   = TCD_WGMODE_ONERAMP_gc;
    TCD0.CMPASET = 0;
    TCD0.CMPACLR = VESA_HSYNC_PULSE_CNT - 1;
    TCD0.CMPBCLR = VESA_HSYNC_PERIOD_CNT - 1;
    TCD0.INTCTRL = TCD_OVF_bm;
    _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD_CMPAEN_bm);
    TCD0.CTRLA   = TCD_CLKSEL_CLKPER_gc | TCD_CNTPRES_DIV1_gc | TCD_SYNCPRES_DIV1_gc;

    while (!(TCD0.STATUS & TCD_ENRDY_bm));
    TCD0.CTRLA |= TCD_ENABLE_bm;
}
void vsync_init(void) {
    // Event: TCD0 end-of-cycle (one pulse per line) -> channel 0 -> TCA0 count input
    EVSYS.CHANNEL0 = EVSYS_CHANNEL0_TCD0_CMPBCLR_gc;
    EVSYS.USERTCA0CNTA = EVSYS_USER_CHANNEL0_gc;

    // Pin: WO0 on PA0, idle high via inversion
    PORTA.DIRSET   = PIN0_bm;
    PORTA.PIN0CTRL = PORT_INVEN_bm;

    // TCA0 counts lines, not clock cycles
    TCA0.SINGLE.CTRLB  = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;
    TCA0.SINGLE.PER    = VESA_FRAME_LINES - 1;    // 524
    TCA0.SINGLE.CMP0   = VSYNC_PULSE_LINES;       // output high for CNT 0..1
    TCA0.SINGLE.EVCTRL = TCA_SINGLE_CNTAEI_bm | TCA_SINGLE_EVACTA_CNT_POSEDGE_gc;
    TCA0.SINGLE.CTRLA  = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}