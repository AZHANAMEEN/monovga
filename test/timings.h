#ifndef TIMINGS_H
#define TIMINGS_H

#define F_CPU_HZ              24000000UL

// VESA 640x480 @ 59.94 Hz timing with negative/negative sync polarity.
// The AVR runs at 24 MHz, so these are the closest practical timer counts.
#define VIDEO_MODE_NAME        "640x480@59.91 (24MHz)"
#define VESA_HSYNC_HZ          31456UL   // 24 MHz / 763
#define VESA_HSYNC_PERIOD_CNT  763U
#define VESA_HSYNC_PULSE_CNT   91U
#define VESA_FRAME_LINES       525U

// Convention A: line 0 = first visible line (pure VESA numbering)
#define VESA_VSYNC_START_LINE  490U
#define VESA_VSYNC_END_LINE    492U

// Convention B: line 0 = start of VSYNC (matches the TCA0 setup)
#define VSYNC_START_LINE       0U
#define VSYNC_END_LINE         2U
#define VISIBLE_START_LINE     35U       // 2 sync + 33 back porch
#define VISIBLE_END_LINE       515U      // 480 visible lines
#define VESA_FRAME_LINES   525U
#define VSYNC_PULSE_LINES  2U
#define VISIBLE_START_LINE  35U
#define VISIBLE_END_LINE    515U
#endif
