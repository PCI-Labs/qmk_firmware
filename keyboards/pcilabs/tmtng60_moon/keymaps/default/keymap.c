/*
Copyright 2024 Mkass420 (@Mkass420)
SPDX-License-Identifier: GPL-2.0-or-later
*/
#include QMK_KEYBOARD_H
#include "config.h"
#include "pcilabs_common/analogkeys.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*[0] = LAYOUT_60_ansi(
        QK_GESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,      KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS,       KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT,            KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_UP, KC_SLSH,
        KC_LCTL,   MO(1),   KC_LALT,                     KC_SPC,                                     KC_LEFT, KC_DOWN, KC_RGHT, KC_LGUI
    ),
*/
    [0] = LAYOUT_60_ansi(
        QK_GESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,      KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS,       KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT,            KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_UP, KC_SLSH,
        KC_LCTL,   MO(1),   KC_LALT,    KC_NO,  KC_NO,   KC_SPC,          KC_NO,     KC_NO,          KC_LEFT, KC_DOWN, KC_RGHT, KC_LGUI
    ),
    [1] = LAYOUT_60_ansi(
        QK_BOOT, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        _______,     RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______, DB_TOGG, EE_CLR,
        _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, QK_RBT,
        _______,            _______, _______, _______, _______, _______, NK_TOGG, _______, _______, _______, KC_SLSH, KC_RSFT,
        _______,   _______,   _______,   KC_NO,   KC_NO,   _______,   KC_NO,   KC_NO,   KC_RALT,   KC_RGUI,   KC_RCTL,   _______
    )
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Row start indices and direction flags
    static const uint8_t row_starts[] = {0, 14, 28, 41, 53};
    static const uint8_t row_ends[] = {13, 27, 40, 52, 64};
    static const bool row_reversed[] = {false, true, false, true, false};

    for (uint8_t i = led_min; i < led_max; i++) {
        // Find which row this LED belongs to
        for (uint8_t row = 0; row < 5; row++) {
            if (i <= row_ends[row]) {
                uint8_t col;
                if (row_reversed[row]) {
                    col = row_ends[row] - i;
                } else {
                    col = i - row_starts[row];
                }
                
                analog_key_t *key = &keys[row][col];
                if (matrix_is_on(row, col)) {
                    // Key is actively pressed - show green
                    rgb_matrix_set_color(i, RGB_GREEN);
                } else if (key->value < 10) {
                    // Key is fully released - turn off LED
                    rgb_matrix_set_color(i, 0, 0, 0);
                } else if (key->value > (key->actuation_point)) {
                    // Key was pressed but now releasing - show red
                    rgb_matrix_set_color(i, RGB_RED);
                } else {
                    // Key is in initial press phase but not activated yet - turn off LED
                    rgb_matrix_set_color(i, 0, 0, 0);
                }
                break;
            }
        }
    }
    return false;
}
