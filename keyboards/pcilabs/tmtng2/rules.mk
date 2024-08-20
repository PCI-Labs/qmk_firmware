MCU_FAMILY = STM32
MCU_SERIES = STM32F1xx

BOARD = STM32_F103_STM32DUINO

MCU_LDSCRIPT = GD32F303xC-dapboot
BOOTLOADER = custom
SRC += dapboot/bootloader.c

DEFAULT_FOLDER = pcilabs/tmtng2
SRC += analog.c
SRC += matrix.c

# Build Options
#   change yes to no to disable
#
CUSTOM_MATRIX = lite
BOOTMAGIC_ENABLE = no      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = no       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
СOMMAND_ENABLE = yes         # Commands for debug and configuration
NKRO_ENABLE = yes            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no      # Enable keyboard backlight functionality
//RGBLIGHT_ENABLE = yes        # Enable keyboard RGB underglow
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = WS2812
AUDIO_ENABLE = no           # Audio output
RAW_ENABLE = yes 
WS2812_DRIVER = bitbang
WAIT_FOR_USB = yes
NO_USB_STARTUP_CHECK = no
LTO_ENABLE = yes
OPT = 2