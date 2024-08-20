MCU_FAMILY = STM32
MCU_SERIES = STM32F1xx

BOARD = STM32_F103_STM32DUINO

MCU_LDSCRIPT = GD32F303xC-dapboot
BOOTLOADER = custom
SRC += dapboot/bootloader.c

DEFAULT_FOLDER = pcilabs/tmtng3v2_new
SRC += analog.c
SRC += matrix.c
SRC += lut.c

# Build Options
#   change yes to no to disable
#
CUSTOM_MATRIX = lite
# Enable N-Key Rollover
#BACKLIGHT_ENABLE = no      # Enable keyboard backlight functionality
#RGBLIGHT_ENABLE = yes        # Enable keyboard RGB underglow
#RGBLIGHT_ENABLE = no
#RGB_MATRIX_ENABLE = yes
#RGB_MATRIX_DRIVER = ws2812
AUDIO_ENABLE = no           # Audio output
RAW_ENABLE = yes
#WS2812_DRIVER = bitbang
WAIT_FOR_USB = yes
NO_USB_STARTUP_CHECK = no
LTO_ENABLE = yes
OPT = 2
