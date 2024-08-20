/*
Copyright 2021 DmNosachev

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define MATRIX_ROW_PINS                                                        \
  { A5, A7 }
#define MATRIX_COL_PINS                                                        \
  { A4, A6, B9 }

#define LED_CTRL_XFER_PIN B6
#define LED_KB_LOCK_PIN B11
#define RAW_USAGE_PAGE 0xFF60
#define RAW_USAGE_ID 0x61

/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* mechanical locking support. NumLock key on the numpad uses Alps SKCL Lock
 * switch */
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
// #define DEBUG_MATRIX_SCAN_RATE
#define FORCE_NKRO

#define RGBLIGHT_EFFECT_BREATHING
#define RGBLIGHT_EFFECT_RAINBOW_MOOD
#define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#define RGBLIGHT_EFFECT_SNAKE
#define RGBLIGHT_EFFECT_KNIGHT
#define RGBLIGHT_EFFECT_CHRISTMAS
#define RGBLIGHT_EFFECT_STATIC_GRADIENT
#define RGBLIGHT_EFFECT_RGB_TEST
#define RGBLIGHT_EFFECT_ALTERNATING
#define RGBLIGHT_EFFECT_TWINKLE
#define RGBLIGHT_SLEEP
#define RGBLED_NUM 65
#define RGBLIGHT_HUE_STEP 8
#define RGBLIGHT_SAT_STEP 8
#define RGBLIGHT_VAL_STEP 8
#define RGBLIGHT_EFFECT_KNIGHT_LED_NUM 65
#define WS2812_DI_PIN A8
#define WS2812_TIMING 1400
#define WS2812_T1H 975
#define WS2812_T0H 425
#define WS2812_TRST_US 350

//#define ENABLE_RGB_MATRIX_BREATHING
//#define RGB_DISABLE_WHEN_USB_SUSPENDED
// The pin connected to the data pin of the LEDs
//#define WS2812_DI_PIN A8
// The number of LEDs connected
//#define RGBLIGHT_SLEEP
//#define RGBLIGHT_LED_COUNT 65
//#define RGB_MATRIX_LED_COUNT 16
#define USB_MAX_POWER_CONSUMPTION 500
#define DEBUG_MATRIX_SCAN_RATE
//#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100
//#define ENABLE_RGB_MATRIX_ALPHAS_MODS
//#define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
//#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
//#define ENABLE_RGB_MATRIX_BREATHING
//#define ENABLE_RGB_MATRIX_BAND_SAT
//#define ENABLE_RGB_MATRIX_BAND_VAL
//#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
//#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
//#define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
//#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
//#define ENABLE_RGB_MATRIX_SOLID_COLOR
//#define ENABLE_RGB_MATRIX_CYCLE_ALL
//#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
//#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
//#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
//#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
//#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
//#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
//#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
//#define ENABLE_RGB_MATRIX_DUAL_BEACON
//#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
//#define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
//#define ENABLE_RGB_MATRIX_RAINDROPS
//#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
//#define ENABLE_RGB_MATRIX_HUE_BREATHING
//#define ENABLE_RGB_MATRIX_HUE_PENDULUM
//#define ENABLE_RGB_MATRIX_HUE_WAVE
//#define ENABLE_RGB_MATRIX_PIXEL_RAIN
//#define ENABLE_RGB_MATRIX_PIXEL_FLOW
//#define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
