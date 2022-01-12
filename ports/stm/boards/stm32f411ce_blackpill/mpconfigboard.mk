USB_VID = 0x239A
USB_PID = 0x806A
USB_PRODUCT = "stm32f411ce blackpill"
USB_MANUFACTURER = "WeAct"

# SPI_FLASH_FILESYSTEM = 1
# EXTERNAL_FLASH_DEVICES = xxxxxx #See supervisor/shared/external_flash/devices.h for options
# LONGINT_IMPL = MPZ

INTERNAL_FLASH_FILESYSTEM = 1

MCU_SERIES = F4
MCU_VARIANT = STM32F411xE
MCU_PACKAGE = UFQFPN48

LD_COMMON = boards/common_default.ld
LD_FILE = boards/STM32F411_fs.ld

# Too big for the flash
CIRCUITPY_AUDIOCORE = 0
CIRCUITPY_AUDIOPWMIO = 0
CIRCUITPY_ULAB = 0
