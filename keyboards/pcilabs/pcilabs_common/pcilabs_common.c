/*
 * pcilabs_common.c
 * 
 * Ensures that idle recalibration (get_sensor_offsets()) only happens
 * when ALL analog keys are truly at rest below a small threshold.
 * 
 * Copyright 2023 RephlexZero (@RephlexZero)
 * Copyright 2024 Mkass420 (@Mkass420)
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "analogkeys.h"
#include "config.h"
#include "quantum.h"
#include "eeprom.h"
#include "scanfunctions.h"
#include "print.h"
#include "rawhid.h"
#include "lut.h"
#include "matrix.h"         // For matrix_scan(), matrix_get_row() if needed
#include "deferred_exec.h"  // For deferred_token, defer_exec(), extend_deferred_exec()

// -----------------------------------------------------------------------------
// External references
// -----------------------------------------------------------------------------
extern const key_config_t key_config_default;
extern lut_t              luts[];

extern uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];

// -----------------------------------------------------------------------------
// Bootmagic (if enabled)
// -----------------------------------------------------------------------------
#ifdef BOOTMAGIC_ENABLE
void bootmagic_scan(void) {
    matrix_scan(); // ensures 'keys' array is updated
    analog_key_t *key = &keys[BOOTMAGIC_ROW][BOOTMAGIC_COLUMN];

    switch_type_t sw        = key->switch_type;
    uint16_t      threshold = distance_to_adc(switch_ranges[sw] * 3 / 4, sw);
    uint16_t      raw_value = key->raw;

    // If above/below the threshold, jump to bootloader
    if (((luts[sw].b > 0) && (raw_value > threshold)) ||
        ((luts[sw].b < 0) && (raw_value < threshold))) {
        bootloader_jump();
    }
}
#endif

// -----------------------------------------------------------------------------
// Debug printing (if DEBUG_ENABLE) and Deferred Exec tokens
// -----------------------------------------------------------------------------
#ifdef DEFERRED_EXEC_ENABLE

#ifdef DEBUG_ENABLE
deferred_token debug_token;

/**
 * Debug print function, prints a row’s key info each time.
 */
bool debug_print(void) {
    static char    rowBuffer[MATRIX_COLS * 25];
    static uint8_t currentRow = 0;
    char          *bufferPtr  = rowBuffer;

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        analog_key_t *key = &keys[currentRow][col];
        if (key->raw) {
            bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer),
                                  "%5u  %5u  %6ld", key->value, key->raw, key->offset);
        } else {
            bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer),
                                  " null   ");
        }
    }

    uprintf("%s\n", rowBuffer);
    currentRow++;

    if (currentRow >= MATRIX_ROWS) {
        currentRow = 0;
        uprintf("\n");
        return false;
    }

    return true;
}


uint32_t debug_print_callback(uint32_t trigger_time, void *cb_arg) {
    debug_print();
    return 100; 
}
#endif 


deferred_token idle_recalibrate_token;


#define ANALOG_AT_REST_THRESHOLD 10


bool all_keys_fully_released(void) {

    matrix_scan();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            analog_key_t *key = &keys[row][col];
         
            if (key->value > ANALOG_AT_REST_THRESHOLD) {
                return false;
            }
        }
    }
    return true;
}


uint32_t idle_recalibrate_callback(uint32_t trigger_time, void *cb_arg) {
    if (!all_keys_fully_released()) {
        return 1000;
    }
    get_sensor_offsets();
    return 1000;
}


bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
 
    extend_deferred_exec(idle_recalibrate_token, 10000);
    return true; 
}

#endif 


void values_save(void) {
    eeconfig_kb_t kb_config;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            memcpy(kb_config.key_configs[row][col].config_raw,
                   keys[row][col].config_raw,
                   EECONFIG_KEY_DATA_SIZE);
        }
    }
    kb_config.magic = eeconfig_magic_const;
    eeconfig_update_kb_datablock(&kb_config.config_raw);
}

void eeconfig_init_kb() {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            memcpy(keys[row][col].config_raw,
                   key_config_default.config_raw,
                   EECONFIG_KEY_DATA_SIZE);
        }
    }
    dynamic_keymap_reset();
    dynamic_keymap_macro_reset();
    values_save();
}

void values_load(void) {
    eeconfig_kb_t kb_config;
    eeconfig_read_kb_datablock(&kb_config.config_raw);
    if (kb_config.magic != eeconfig_magic_const) {
        eeconfig_init_kb();
        return;
    }

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            memcpy(keys[row][col].config_raw,
                   kb_config.key_configs[row][col].config_raw,
                   EECONFIG_KEY_DATA_SIZE);
        }
    }
}

// -----------------------------------------------------------------------------
// keyboard_post_init_kb
// -----------------------------------------------------------------------------
void keyboard_post_init_kb(void) {
    // Initial offset measurement
    get_sensor_offsets();

#ifdef DEFERRED_EXEC_ENABLE
#    ifdef DEBUG_ENABLE
    // Start periodic debug printing every 1000 ms
    debug_token = defer_exec(1000, debug_print_callback, NULL);
#    endif

    // Schedule first idle recalibration 3 seconds after startup
    // Then it repeats at the interval returned by idle_recalibrate_callback()
    idle_recalibrate_token = defer_exec(3000, idle_recalibrate_callback, NULL);
#endif

    // Load user-stored values from EEPROM
    values_load();

    // Call user-level post init if defined
    keyboard_post_init_user();
}

// -----------------------------------------------------------------------------
// process_record_user
// -----------------------------------------------------------------------------
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef CONSOLE_ENABLE
    uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n",
            keycode,
            record->event.key.col,
            record->event.key.row,
            record->event.pressed,
            record->event.time,
            record->tap.interrupted,
            record->tap.count);
#endif
    return true;
}
