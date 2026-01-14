/*
 * Copyright (c) 2024
 * SPDX-License-Identifier: MIT
 *
 * status led driver for ws2812 addressable led
 * date: 2026-01-14
 */

#ifndef STATUS_LED_H_
#define STATUS_LED_H_

#include <stdint.h>

// led status states
typedef enum {
    STATUS_LED_USB_CONNECTED,   // yellow - usb enumerated, no dap
    STATUS_LED_DAP_CONNECTED,   // green - debugger connected
    STATUS_LED_DAP_RUNNING,     // blue - target running
    STATUS_LED_DAP_ACTIVITY     // purple pulse - dap transfer activity
} status_led_state_t;

// initialize the ws2812 status led on the configured gpio
void status_led_init(void);

// set the led to a specific state
void status_led_set_state(status_led_state_t state);

// brief activity pulse (call during dap transfers)
void status_led_activity(void);

#endif // STATUS_LED_H_
