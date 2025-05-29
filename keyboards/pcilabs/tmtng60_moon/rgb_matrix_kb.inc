// !!! DO NOT ADD #pragma once !!! //
#include QMK_KEYBOARD_H
#include "config.h"
#include "pcilabs_common/analogkeys.h"
// Step 1: Declare the custom effect
RGB_MATRIX_EFFECT(analog_visualizer)

// Step 2: Define the effect implementation
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool analog_visualizer(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

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
    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS 