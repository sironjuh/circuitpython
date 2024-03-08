USB_VID = 0x303A
USB_PID = 0x815D
USB_PRODUCT = "Bee-Data-Logger"
USB_MANUFACTURER = "Smart Bee Designs"

IDF_TARGET = esp32s3

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 8MB


FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DS3231
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SD
CIRCUITPY_ESPCAMERA = 0
