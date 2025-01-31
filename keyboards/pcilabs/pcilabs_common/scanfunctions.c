/*
Copyright 2023 RephlexZero (@RephlexZero)
Copyright 2024 Mkass420 (@Mkass420)
SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "scanfunctions.h"
#include <stdint.h>
#include "config.h"
#include "lut.h"
#include "printf.h"
#include "custom_analog.h"
#include "analogkeys.h"

void lut_init(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            analog_key_t *key = &keys[row][col];
            key->lut          = &luts[key->switch_type].values;
        }
    }
}

void get_sensor_offsets(void) {
    matrix_scan();
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            analog_key_t  *key            = &keys[row][col];
            const uint16_t rest_adc_value = distance_to_adc(0, key->switch_type);
            key->offset                   = rest_adc_value - key->raw;
            if (key->offset < -230) {
                key->switch_type = latenpow;    
            } else if (key->offset > -200) {
                key->switch_type = KS_20; 
            }
            
       //      if (key->offset < -200) {
         //       key->switch_type = KS_20; 
           //  } 
            key->lut = &luts[key->switch_type].values;
            
        }
    }
}


void update_extremum(analog_key_t *key) {
    key->extremum = key->value;
}

void register_key(matrix_row_t *current_row, uint8_t current_col) {
    *current_row |= (1 << current_col);
}

void deregister_key(matrix_row_t *current_row, uint8_t current_col) {
    *current_row &= ~(1 << current_col);
}

void matrix_read_cols_static_actuation(matrix_row_t *current_row, uint8_t current_col, analog_key_t *key) {
    if (*current_row & (1 << current_col)) {
        if (key->value <= MAX(key->actuation_point - key->release_hysteresis, 0)) {
            deregister_key(current_row, current_col);
        }
    } else {
        if (key->value >= MIN(key->actuation_point + key->press_hysteresis, switch_ranges[key->switch_type])) {
            register_key(current_row, current_col);
        }
    }
}

/*
void matrix_read_cols_dynamic_keystroke(matrix_row_t *current_row, uint8_t current_col, analog_key_t *key) {
    if (*current_row & (1 << current_col)) {
        if (key->value <= MAX(key->actuation_point - key->release_hysteresis, 0)) {
            deregister_key(current_row, current_col);
        }
    } else {
        if (key->value >= MIN(key->actuation_point + key->press_hysteresis, switch_ranges[key->switch_type])) {
            register_key(current_row, current_col);
        }
    }
}
*/

/* Extremum is the highest or lowest value of the key reached
this functionality changes based on whether the key is pressed or not,
assuming your sensor value decreases when the key is pressed,
if the key is pressed, the extremum is the lowest value reached,
if the key is not pressed, the extremum is the highest value reached. */

void matrix_read_cols_dynamic_actuation(matrix_row_t *current_row, uint8_t current_col, analog_key_t *key) {
    if (key->value > key->actuation_point) {
        /* In DA zone? */
         if ((*current_row) & (1 << current_col)) {
            // Key is pressed
            if (key->value > key->extremum) {
                update_extremum(key);
            } else {
                // Normal release threshold
                int16_t release_threshold = key->extremum - key->release_sensitivity;
                       
                if (key->extremum >= 330) {
                    int16_t clamp_val = key->extremum - key->deadzone; // e.g. extremum - 20
                    if (release_threshold > clamp_val) {
                        release_threshold = clamp_val;
                    }
                }

                // Normal release check
                if (key->value <= release_threshold) {
                    deregister_key(current_row, current_col);
                    update_extremum(key);
                }
            }
        } else {
            /* Key is not pressed
            Is the key still moving up? */
            if (key->value < key->extremum) {
                update_extremum(key);
            } else if (key->value >= MIN(key->extremum + key->press_sensitivity, switch_ranges[key->switch_type])) {
                /* Has key moved down enough to be pressed? */
                register_key(current_row, current_col);
                update_extremum(key);
            }
        }
    } else {
        /* Out of DA zone
        Always deregister key */
        deregister_key(current_row, current_col);
        if (key->value > key->extremum) {
            update_extremum(key);
        }
    }
}




void matrix_read_cols_continuous_dynamic_actuation(
    matrix_row_t *current_row,
    uint8_t current_col,
    analog_key_t *key
) {
    // 1) Check if we should enter CDA zone
    if (!key->in_cda_zone && key->value > key->actuation_point) {
        key->in_cda_zone = true;
    }

    // 2) If in CDA zone, do dynamic logic
    if (key->in_cda_zone) {
        // If the key is currently pressed in the matrix
        if ((*current_row) & (1 << current_col)) {
            // Key is pressed
            if (key->value > key->extremum) {
                update_extremum(key);
            } else {
                // Normal release threshold
                int16_t release_threshold = key->extremum - key->release_sensitivity;

                // ---------------------------------------
                // ONLY apply the 20 offset if "near top"
                // ---------------------------------------
                // Suppose we say "top" = any extremum >= 300
                if (key->extremum >= 350) {
                    int16_t clamp_val = key->extremum - key->deadzone; // e.g. extremum - 20
                    if (release_threshold > clamp_val) {
                        release_threshold = clamp_val;
                    }
                }

                // Normal release check
                if (key->value <= release_threshold) {
                    deregister_key(current_row, current_col);
                    update_extremum(key);
                }
            }
        } else {
            // Key is NOT pressed
            if (key->value < key->extremum) {
                update_extremum(key);
            } else if (key->value >= MIN(key->extremum + key->press_sensitivity,
                                         switch_ranges[key->switch_type])) {
                register_key(current_row, current_col);
                update_extremum(key);
            }
        }

        // 3) Possibly exit the CDA zone if fully unpressed
        if (key->value <= 10) {
            key->in_cda_zone = false;
            deregister_key(current_row, current_col);

            if (key->value > key->extremum) {
                update_extremum(key);
            }
        }
    }
    else {
        // If not in CDA zone, normal logic
        deregister_key(current_row, current_col);

        if (key->value > key->extremum) {
            update_extremum(key);
        }
    }
}

void (*matrix_read_mode_array[7])(matrix_row_t*, uint8_t, analog_key_t*) = {
    [static_actuation]             = matrix_read_cols_static_actuation,
    [dynamic_actuation]            = matrix_read_cols_dynamic_actuation,
    [continuous_dynamic_actuation] = matrix_read_cols_continuous_dynamic_actuation,
    [dynamic_keystroke]            = NULL
};
