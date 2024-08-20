/* Copyright 2023 PCI Labs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "bootloader.h"
#include "color.h"
#include "quantum.h"
#include <stdint.h>
#define DEBUG

#include "config.h"
#include QMK_KEYBOARD_H
#include "analog.h"
#include "eeprom.h"
#include "print.h"
#include "raw_hid.h"
#include "stdint.h"
typedef enum { OFF, RT, CRT } RT_STATE;

uint32_t pins[] = HE_PINS;

typedef union {
  uint8_t raw[STRUCT_SIZE];
  struct {
    uint8_t init_flag;
    uint16_t rest_positions[KEY_NUM];
    uint16_t down_positions[KEY_NUM];
    uint8_t key_chars[KEY_NUM];
    uint8_t rgb_mode;
    HSV rgb_color;
    uint8_t rgb_speed;
    // uint8_t rgb_red;
    // uint8_t rgb_green;
    // uint8_t rgb_blue;
    uint8_t rapidTriggerSensivityUp[KEY_NUM];
    uint8_t rapidTriggerSensivityDown[KEY_NUM];
    uint16_t actuation_point[KEY_NUM];
    uint8_t dead_zone[KEY_NUM];
    RT_STATE rapid_trigger_state[KEY_NUM];
  };
} user_config_t;

user_config_t user_config;
user_config_t *user_config_ptr = &user_config;

user_config_t default_config = DEFAULT_CONFIG;

uint16_t positions_difference[KEY_NUM];

int constrain(int value, int min, int max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  } else {
    return value;
  }
}

#if SMA_FILTER_ENABLED == TRUE
uint32_t sum_SMA[KEY_NUM] = EMPTY_ARRAY_ZERO;
uint32_t buf_SMA[KEY_NUM][SMA_SAMPLE_SIZE];
uint16_t index_SMA[KEY_NUM] = EMPTY_ARRAY_ZERO;
uint16_t SMA_filter(uint16_t value);
void SMA_init(void);
#endif

#if AUTOCALIBRATION_ENABLED == TRUE
uint32_t sum_SMA_down[KEY_NUM] = EMPTY_ARRAY_ZERO;
uint32_t buf_SMA_down[KEY_NUM][AUTOCALIBRATION_SAMPLE_SIZE];
uint16_t index_SMA_down[KEY_NUM] = EMPTY_ARRAY_ZERO;
void autocalibration_init(void);
void autocalibration(uint16_t value);
#endif

#ifdef DEBUG
static uint16_t timer_debug[KEY_NUM];
#endif

void read_keyboard(void);
void read_key(void);
void handle_traditional(uint16_t value);
void handle_RT(uint16_t value);
void handle_CRT(uint16_t value);
void press_key(void);
void release_key(void);
// void calibration(void);
void calibration_rest(void);
// void calibration_down(void);
void write_firmware_version(void);

typedef struct currentKeyState {
  bool pressed;
  bool inZone;
  uint16_t peak;
} currentKeyState_t;

typedef currentKeyState_t currentKeyStates_t[KEY_NUM];

const uint8_t RGBLED_RAINBOW_MOOD_INTERVALS[] PROGMEM = {30, 5, -5};

int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void write_firmware_version(void) {
  for (size_t i = 0; i < sizeof(FIRMWARE_VERSION); i++) {
    eeprom_write_byte((uint8_t *)FIRMWARE_VERSION_EEPROM_OFFSET + i,
                      FIRMWARE_VERSION[i]);
  }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_Z, KC_X, KC_NO, KC_NO, KC_NO, KC_NO),
};

void config_update_user(uint8_t *raw) {
  for (uint8_t i = 0; i < STRUCT_SIZE; i++) {
    eeprom_write_byte((uint8_t *)EEPROM_OFFSET + i, raw[i]);
  }
  memcpy(user_config.raw, raw, STRUCT_SIZE);
}

void config_init_user(void) { config_update_user(default_config.raw); }

void config_read_user(void) {
  uint8_t raw[STRUCT_SIZE];
  if (eeprom_read_byte(EEPROM_OFFSET) == 0x42) {
    for (uint8_t i = 0; i < STRUCT_SIZE; i++) {
      raw[i] = eeprom_read_byte((uint8_t *)EEPROM_OFFSET + i);
    }
  } else {
    config_init_user();
  }
  memcpy(user_config.raw, raw, STRUCT_SIZE);
  // rgblight_mode_noeeprom(user_config.rgb_mode);
  // rgblight_sethsv_noeeprom(user_config.rgb_hue, user_config.rgb_sat,
  // user_config.rgb_val);
  rgb_matrix_enable();
  rgb_matrix_sethsv(user_config.rgb_color.h, user_config.rgb_color.s,
                    user_config.rgb_color.v);
  rgb_matrix_set_speed_noeeprom(user_config.rgb_speed);
  rgb_matrix_mode_noeeprom(user_config.rgb_mode);
}

void matrix_init_user(void) {
  write_firmware_version();
  config_read_user();
  rgb_matrix_enable();
  rgb_matrix_sethsv(user_config.rgb_color.h, user_config.rgb_color.s,
                    user_config.rgb_color.v);
  rgb_matrix_set_speed_noeeprom(user_config.rgb_speed);
  rgb_matrix_mode_noeeprom(user_config.rgb_mode);
  // rgblight_mode_noeeprom(user_config.rgb_mode);
  // rgblight_sethsv_noeeprom(user_config.rgb_hue, user_config.rgb_sat,
  // user_config.rgb_val);

#if SMA_FILTER_ENABLED == TRUE
  SMA_init();
#endif

#ifdef DEBUG
  for (size_t i = 0; i < KEY_NUM; i++) {
    timer_debug[i] = timer_read();
  }
#endif
  calibration_rest();
#if AUTOCALIBRATION_ENABLED == TRUE
  autocalibration_init();
#endif
}

uint16_t key_index = 0; 
void matrix_scan_user(void) { read_keyboard(); }
/*

*/

const int effects_array[] = {RGB_MATRIX_SOLID_COLOR, RGB_MATRIX_BREATHING, RGB_MATRIX_RAINBOW_BEACON, RGB_MATRIX_CYCLE_ALL, RGB_MATRIX_CYCLE_LEFT_RIGHT, RGB_MATRIX_RAINBOW_MOVING_CHEVRON, RGB_MATRIX_PIXEL_RAIN, RGB_MATRIX_RAINDROPS, RGB_MATRIX_CYCLE_UP_DOWN};

void raw_hid_receive(uint8_t *data, uint8_t length) {
  switch (data[0]) {
  case 1:;
    
  //  user_config.rgb_mode = RGB_MATRIX_SOLID_COLOR;
    user_config.rgb_color.h = data[1];
    user_config.rgb_color.s = data[2];
    user_config.rgb_color.v = data[3];
    user_config.rgb_mode = effects_array[data[4]];
    user_config.rgb_speed = data[5];
    rgb_matrix_sethsv(user_config.rgb_color.h, user_config.rgb_color.s,
                      user_config.rgb_color.v);
    rgb_matrix_mode_noeeprom(user_config.rgb_mode);
    rgb_matrix_set_speed_noeeprom(user_config.rgb_speed);
    config_update_user(user_config.raw);
    break;

  case 2:;
    RT_STATE state = data[1];
    if (data[2]) {
      state = CRT;
    }
    uint8_t sens_up = data[3];
    uint8_t sens_down = data[4];
    uint8_t act = data[5] * 10;
    uint8_t dead_zone = data[6];
    if (act == 10)
    {
     act = 15; 
    } 
    for (size_t i = 0; i < KEY_NUM; i++) {
      user_config.rapid_trigger_state[i] = state;
      user_config.rapidTriggerSensivityUp[i] = sens_up;
      user_config.rapidTriggerSensivityDown[i] = sens_down;
      user_config.actuation_point[i] = act;
      user_config.dead_zone[i] = dead_zone;
      config_update_user(user_config.raw);
    }
    break;

  case 3:;
    user_config.key_chars[0] = data[1];
    user_config.key_chars[1] = data[2];
#if KEY_NUM == 3
    user_config.key_chars[2] = data[3];
#endif
    config_update_user(user_config.raw);
    break;

  case 4:;
    bootloader_jump();
    break;

  case 5:;
    eeconfig_init();
    reset_keyboard();
    break;

  case 6:;
    uint8_t data3 = 7;
    raw_hid_send(&data3, sizeof(32));
    break;

    case 7:;
    uint8_t data2[] = {7, 2, 7};
    raw_hid_send(data2, length);
    break;

    case 8:;
    raw_hid_send(data, length);
    break;
  }
}

currentKeyStates_t currentKeyStates[KEY_NUM] = DEFAULT_KEY_STATES;

void read_keyboard() {
  for (key_index = 0; key_index < KEY_NUM; key_index++) {
    read_key();
  }
}

void read_key() {
  int16_t raw = analogReadPin(pins[key_index]);
#if SMA_FILTER_ENABLED == TRUE
  raw = SMA_filter((uint16_t)raw);
#endif
#if AUTOCALIBRATION_ENABLED == TRUE
  if (raw < user_config_ptr->down_positions[key_index]) {
    autocalibration(raw);
  }
#endif
  uint16_t value = constrain(raw, user_config_ptr->down_positions[key_index],
                             user_config_ptr->rest_positions[key_index]);
  value =
      map(value, user_config_ptr->down_positions[key_index],
          user_config_ptr->rest_positions[key_index], 0, KEY_TRAVEL_DISTANCE);
#ifdef DEBUG
  if (timer_elapsed(timer_debug[key_index]) > 500) {
    /*
    uprintf("scannning key number %d\n", key_index);
    uprintf("raw: %d\n", raw);
    uprintf("value: %d\n", value);
    uprintf("rest: %d\n", user_config_ptr->rest_positions[key_index]);
    uprintf("down: %d\n", user_config_ptr->down_positions[key_index]);
    uprintf("actuation point: %d\n",
            user_config_ptr->actuation_point[key_index]);
            */
    uprintf("%d %d %d\n", key_index, raw, value);
    
    timer_debug[key_index] = timer_read();
  }
#endif
  switch (user_config_ptr->rapid_trigger_state[key_index]) {
  case OFF:
    handle_traditional(value);
    break;
  case RT:
    handle_RT(value);
    break;
  case CRT:
    handle_CRT(value);
    break;
  }
}

void handle_traditional(uint16_t value) {
  if (value <=
      KEY_TRAVEL_DISTANCE - user_config_ptr->actuation_point[key_index])
    press_key();
  else
    release_key();
}

void handle_RT(uint16_t value) {
  /*
  if (value >=
      KEY_TRAVEL_DISTANCE - user_config_ptr->actuation_point[key_index]) {
    currentKeyStates[key_index]->inZone = false;
  }

  if (value <=
          KEY_TRAVEL_DISTANCE - user_config_ptr->actuation_point[key_index] &&
      !currentKeyStates[key_index]->inZone) {
    press_key();
    currentKeyStates[key_index]->inZone = true;
  }

  else if (!currentKeyStates[key_index]->pressed &&
           currentKeyStates[key_index]->inZone &&
           value + user_config_ptr->rapidTriggerSensivityDown[key_index] <=
               currentKeyStates[key_index]->peak) {
    press_key();
  } else if (currentKeyStates[key_index]->pressed &&
             (!currentKeyStates[key_index]->inZone ||
              value >=
                  (currentKeyStates[key_index]->peak +
                             user_config_ptr->rapidTriggerSensivityUp[key_index]))) {
    release_key();
  }
  if ((currentKeyStates[key_index]->pressed &&
       value < currentKeyStates[key_index]->peak) ||
      (!currentKeyStates[key_index]->pressed &&
       value > currentKeyStates[key_index]->peak)) {
    currentKeyStates[key_index]->peak = value;
  }
  */
  if (value >= KEY_TRAVEL_DISTANCE - 10) {
    currentKeyStates[key_index]->inZone = false;
  }

  if (value <=
          KEY_TRAVEL_DISTANCE - user_config_ptr->actuation_point[key_index] &&
      !currentKeyStates[key_index]->inZone) {
    press_key();
    currentKeyStates[key_index]->inZone = true;
  }

  else if (!currentKeyStates[key_index]->pressed &&
           currentKeyStates[key_index]->inZone &&
           value + user_config_ptr->rapidTriggerSensivityDown[key_index] <=
               currentKeyStates[key_index]->peak) {
    press_key();
  } else if (currentKeyStates[key_index]->pressed &&
             (!currentKeyStates[key_index]->inZone ||
              value >=
                  currentKeyStates[key_index]->peak +
                      user_config_ptr->rapidTriggerSensivityUp[key_index])) {
    release_key();
  }
  if ((currentKeyStates[key_index]->pressed &&
       value < currentKeyStates[key_index]->peak) ||
      (!currentKeyStates[key_index]->pressed &&
       value > currentKeyStates[key_index]->peak)) {
    currentKeyStates[key_index]->peak = value;
  }
}

void handle_CRT(uint16_t value) {
 
  if (value >= KEY_TRAVEL_DISTANCE - 10 ||
      value <= user_config_ptr->dead_zone[key_index]) {
    currentKeyStates[key_index]->inZone = false;
  }

  if (value <=
          KEY_TRAVEL_DISTANCE - user_config_ptr->actuation_point[key_index] &&
      !currentKeyStates[key_index]->inZone) {
    press_key();
    currentKeyStates[key_index]->inZone = true;
  }

  else if (!currentKeyStates[key_index]->pressed &&
           currentKeyStates[key_index]->inZone &&
           value + user_config_ptr->rapidTriggerSensivityDown[key_index] <=
               currentKeyStates[key_index]->peak) {
    press_key();
  } else if (currentKeyStates[key_index]->pressed &&
             (!currentKeyStates[key_index]->inZone ||
              value >=
                  currentKeyStates[key_index]->peak +
                      user_config_ptr->rapidTriggerSensivityUp[key_index])) {
    release_key();
  }
  if ((currentKeyStates[key_index]->pressed &&
       value < currentKeyStates[key_index]->peak) ||
      (!currentKeyStates[key_index]->pressed &&
       value > currentKeyStates[key_index]->peak)) {
    currentKeyStates[key_index]->peak = value;
  }
}
void press_key() {
  if (currentKeyStates[key_index]->pressed)
    return;
  register_code(user_config_ptr->key_chars[key_index]);
  currentKeyStates[key_index]->pressed = true;
}

void release_key(void) {
  if (!currentKeyStates[key_index]->pressed)
    return;
  unregister_code(user_config_ptr->key_chars[key_index]);
  currentKeyStates[key_index]->pressed = false;
}
void calibration_rest(void) {
  uint32_t calibration_rest_positions[KEY_NUM] = EMPTY_ARRAY_ZERO;
  uint16_t rest_positions_avg[KEY_NUM];
  for (size_t key_index = 0; key_index < KEY_NUM; key_index++) {
    for (size_t sample = 0; sample < 128; sample++) {
      calibration_rest_positions[key_index] +=
          analogReadPin(pins[key_index]) - 5;
    }
    rest_positions_avg[key_index] = calibration_rest_positions[key_index] >> 7;
    positions_difference[key_index] =
        (rest_positions_avg[key_index] + REST_POS_MAGIC) -
        (user_config_ptr->down_positions[key_index] + DOWN_POS_MAGIC);
  }
  memcpy(user_config_ptr->rest_positions, rest_positions_avg,
         sizeof(rest_positions_avg));
}
/*
void calibration_down(void) {
  uint32_t calibration_down_positions[KEY_NUM] = EMPTY_ARRAY_ZERO;
  uint16_t down_positions_avg[KEY_NUM];
  for (size_t key_index = 0; key_index < KEY_NUM; key_index++) {
    for (size_t sample = 0; sample < 128; sample++) {
      calibration_down_positions[key_index] +=
          analogReadPin(pins[key_index] + 5);
    }
    down_positions_avg[key_index] = calibration_down_positions[key_index] >> 7;
    positions_difference[key_index] =
        (user_config_ptr->rest_positions[key_index] + REST_POS_MAGIC) -
        (down_positions_avg[key_index] + DOWN_POS_MAGIC);
  }
  memcpy(user_config_ptr->down_positions, down_positions_avg,
         sizeof(down_positions_avg));
  config_update_user(user_config.raw);
}
*/
#if SMA_FILTER_ENABLED == TRUE
void SMA_init(void) {
  for (size_t i = 0; i < KEY_NUM; i++) {
    for (size_t j = 0; j < SMA_SAMPLE_SIZE; j++) {
      uint16_t value = analogReadPin(pins[i]);
      sum_SMA[i] += value;
      buf_SMA[i][j] = value;
    }
  }
}
uint16_t SMA_filter(uint16_t value) {
  sum_SMA[key_index] =
      sum_SMA[key_index] - buf_SMA[key_index][index_SMA[key_index]] + value;
  buf_SMA[key_index][index_SMA[key_index]] = value;
  index_SMA[key_index]++;
  if (index_SMA[key_index] == SMA_SAMPLE_SIZE)
    index_SMA[key_index] = 0;
  return sum_SMA[key_index] >> SMA_SAMPLE_EXP;
}
#endif
#if AUTOCALIBRATION_ENABLED == TRUE
void autocalibration_init(void) {
  int32_t base_difference[KEY_NUM];
  for (size_t i = 0; i < KEY_NUM; i++) {
    base_difference[i] =
        user_config_ptr->rest_positions[i] - REST_CALIBRATION_BASE;
    user_config_ptr->down_positions[i] =
        DOWN_CALIBRATION_BASE + base_difference[i];
    for (size_t j = 0; j < AUTOCALIBRATION_SAMPLE_SIZE; j++) {
      sum_SMA_down[i] += user_config_ptr->down_positions[i];
      buf_SMA_down[i][j] = user_config_ptr->down_positions[i];
    }
  }
}
void autocalibration(uint16_t value) {
  sum_SMA_down[key_index] = sum_SMA_down[key_index] -
                            buf_SMA_down[key_index][index_SMA_down[key_index]] +
                            value + 3;
  buf_SMA_down[key_index][index_SMA_down[key_index]] = value + 3;
  index_SMA_down[key_index]++;
  if (index_SMA_down[key_index] == AUTOCALIBRATION_SAMPLE_SIZE)
    index_SMA_down[key_index] = 0;
  user_config_ptr->down_positions[key_index] =
      (sum_SMA_down[key_index] >> AUTOCALIBRATION_SAMPLE_EXP);
}
#endif

void keyboard_post_init_user(void) {
  // rgblight_enable_noeeprom();
  // rgblight_mode_noeeprom(user_config.rgb_mode);
  // rgblight_sethsv_noeeprom(user_config.rgb_hue, user_config.rgb_sat,
  // user_config.rgb_val);
  rgb_matrix_enable();
  rgb_matrix_mode_noeeprom(user_config.rgb_mode);
  rgb_matrix_set_speed_noeeprom(user_config.rgb_speed);
  rgb_matrix_sethsv(user_config.rgb_color.h, user_config.rgb_color.s,
                    user_config.rgb_color.v);
}