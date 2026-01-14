/*
 * Copyright (c) 2024
 * SPDX-License-Identifier: MIT
 *
 * status led driver for ws2812 addressable led
 * uses pio1 to avoid interference with debug functionality on pio0
 * date: 2026-01-14
 */

#include "status_led.h"
#include "probe_config.h"

#include <hardware/pio.h>
#include <hardware/clocks.h>
#include "ws2812.pio.h"

#ifdef PROBE_STATUS_LED_WS2812

// use pio1 to avoid interference with probe/autobaud on pio0
static PIO status_pio = pio1;
static uint status_sm;
static uint status_offset;

// grb color format for ws2812
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(g) << 24) |
           ((uint32_t)(r) << 16) |
           ((uint32_t)(b) << 8);
}

// color definitions (grb format, shifted for 24-bit output)
#define COLOR_YELLOW urgb_u32(127, 90, 0)   // usb connected (50% brightness)
#define COLOR_GREEN  urgb_u32(0, 127, 0)     // dap connected (50% brightness)
#define COLOR_BLUE   urgb_u32(0, 0, 127)     // running (50% brightness)
#define COLOR_PURPLE urgb_u32(127, 0, 127)   // activity pulse (50% brightness)

static status_led_state_t current_state = STATUS_LED_USB_CONNECTED;
static uint32_t activity_counter = 0;

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(status_pio, status_sm, pixel_grb);
}

void status_led_init(void) {
    // claim a state machine on pio1
    status_sm = pio_claim_unused_sm(status_pio, true);
    
    // add the ws2812 program
    status_offset = pio_add_program(status_pio, &ws2812_program);
    
    // initialize at 800khz (ws2812 standard frequency)
    ws2812_program_init(status_pio, status_sm, status_offset, 
                        PROBE_STATUS_LED_PIN, 800000, false);
    
    // set initial state
    status_led_set_state(STATUS_LED_USB_CONNECTED);
}

void status_led_set_state(status_led_state_t state) {
    current_state = state;
    
    uint32_t color;
    switch (state) {
        case STATUS_LED_DAP_CONNECTED:
            color = COLOR_GREEN;
            break;
        case STATUS_LED_DAP_RUNNING:
            color = COLOR_BLUE;
            break;
        case STATUS_LED_DAP_ACTIVITY:
            color = COLOR_PURPLE;
            break;
        case STATUS_LED_USB_CONNECTED:
        default:
            color = COLOR_YELLOW;
            break;
    }
    
    put_pixel(color);
}

void status_led_activity(void) {
    // toggle between purple and current base state every few calls
    // this creates a visible flicker during rapid transfers
    activity_counter++;
    if (activity_counter % 4 == 0) {
        put_pixel(COLOR_PURPLE);
    } else if (activity_counter % 4 == 2) {
        // restore to current state
        status_led_set_state(current_state);
    }
}

#else

// stub implementations when ws2812 not configured
void status_led_init(void) {}
void status_led_set_state(status_led_state_t state) { (void)state; }
void status_led_activity(void) {}

#endif // PROBE_STATUS_LED_WS2812

