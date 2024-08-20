
/*qmk defines*/
#define USB_POLLING_INTERVAL_MS 1
#define ADC_RESOLUTION ADC_CFGR1_RES_12BIT
#define DEBOUNCE 1
#define STM32_ONBOARD_EEPROM_SIZE

/*firmware version*/
#define FIRMWARE_VERSION "1.0.0"
#define FIRMWARE_VERSION_EEPROM_OFFSET 128

/*keypad hardware*/
#define KEY_NUM 2
#define KEY_TRAVEL_DISTANCE 410  // 0.01 mm
#define HALL_SENSOR_DISTANCE 160 // 0.01 mm

/*magical constants for mapping*/
#define DOWN_POS_MAGIC -490
#define REST_POS_MAGIC 92
#define MAP_MAGIC -39

/*SMA settings*/
#define SMA_FILTER_ENABLED TRUE
#if SMA_FILTER_ENABLED == TRUE
#define SMA_SAMPLE_EXP 5
#define SMA_SAMPLE_SIZE (1 << SMA_SAMPLE_EXP)
#endif

/*calibration settings*/
#define AUTOCALIBRATION_ENABLED TRUE
#if AUTOCALIBRATION_ENABLED == TRUE
#define REST_CALIBRATION_BASE 1800
#define DOWN_CALIBRATION_BASE 1100
#define AUTOCALIBRATION_SAMPLE_EXP 5
#define AUTOCALIBRATION_SAMPLE_SIZE (1 << AUTOCALIBRATION_SAMPLE_EXP)
#endif

/*rawhid settings*/
#define NEW_REPORTS TRUE

/*user config*/
#define STRUCT_SIZE 64
#define EEPROM_OFFSET ((uint8_t *)64)

/*defaults*/

#define HE_PINS                                                                \
  { A2, A1 }

#define EMPTY_ARRAY_ZERO                                                       \
  { 0, 0 }
#define EMPTY_ARRAY_UINT16_MAX                                                 \
  { UINT16_MAX, UINT16_MAX }
#define DEFAULT_CONFIG                                                         \
  {                                                                            \
    .init_flag = 0x42, .rest_positions = {1800, 1800},                         \
    .down_positions = {1300, 1300}, .key_chars = {KC_Z, KC_X},                 \
    .rapidTriggerSensivityUp = {30, 30},                                       \
    .rapidTriggerSensivityDown = {30, 30}, .rgb_mode = 5, .rgb_speed = 127,    \
    .rgb_color = {255, 255, 255}, .actuation_point = {30, 30},                 \
    .dead_zone = {20, 20},                                                     \
    .rapid_trigger_state = {                                                   \
      CRT,                                                                     \
      CRT                                                                      \
    }                                                                          \
  }
// clang-format off
#define DEFAULT_KEY_STATES                                     \
  {                                                            \
    {{.pressed = false, .inZone = false, .peak = UINT16_MAX}}, \
    {{.pressed = false, .inZone = false, .peak = UINT16_MAX}}  \
  }
// clang-format on
