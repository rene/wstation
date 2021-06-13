SKETCH = main.ino

UPLOAD_PORT ?= /dev/ttyUSB0
UPLOAD_SPEED ?= 921600
BOARD ?= esp32doit-devkit-v1
CHIP ?= esp32

BUILD_DIR ?= $(PWD)/build
ESP_LIBS ?= $(shell echo $HOME)/.arduino15/packages/esp32/hardware/esp32/1.0.4/libraries

DEBUG_LEVEL ?= 5

FS_DIR = $(PWD)/fsroot

# RTC_DS1307 = true # Enable RTC support
RTC_DS1307 ?=

# DHT_SENSOR = true # To use DHT temperature/humidity sensor
DHT_SENSOR ?=

# HTU2X_SENSOR = true # To use HTU2x temperature/humidity sensor
HTU2X_SENSOR ?= true

# Use -DDEBUG_SCREENSHOT=1 to enable screenshot support
ENABLE_DEBUG_SCREENSHOT ?=

# Versioning
GIT_DESC=$(shell git describe --tags --long)
WSVERSION=$(GIT_DESC)

BUILD_EXTRA_FLAGS += -I./include -Wall -DWSTATION_VERSION=\"${WSVERSION}\" -DCORE_DEBUG_LEVEL=$(DEBUG_LEVEL) $(ENABLE_DEBUG_SCREENSHOT)

LIBS=$(ESP_LIBS)/Wire \
	 $(ESP_LIBS)/SPI \
	 $(ESP_LIBS)/WiFi \
	 $(ESP_LIBS)/WiFiClientSecure \
	 $(ESP_LIBS)/HTTPClient \
	 $(ESP_LIBS)/FS \
	 $(ESP_LIBS)/SPIFFS \
	 $(ESP_LIBS)/Update \
	 libs/AsyncTCP \
	 libs/ESPAsyncWebServer \
	 libs/ArduinoJson-6.15.1/ \
	 libs/Adafruit_GFX_Library-1.7.5 \
	 libs/Adafruit_ILI9341-1.5.4 \
	 libs/Time \
	 libs/ArduinoNvs

ifeq ($(RTC_DS1307),true)
LIBS += libs/DS1307RTC
BUILD_EXTRA_FLAGS += -DRTC_DS1307
endif

ifeq ($(DHT_SENSOR),true)
LIBS += libs/DHTesp
BUILD_EXTRA_FLAGS += -DDHT_SENSOR
endif

ifeq ($(HTU2X_SENSOR),true)
LIBS += libs/HTU21D_LIB
BUILD_EXTRA_FLAGS += -DHTU2X_SENSOR
endif

PNGDIR=../resources/icons/png

.PHONY: png clean-png

include makeEspArduino.mk

# Convert PNG files to .px (RGB565)
png:
	@mkdir -p $(FS_DIR)
	@$(PNGDIR)/convert_to_file.py $(PNGDIR) $(FS_DIR)

clean-png:
	@rm -rf $(PNGCDIR)
