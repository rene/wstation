SKETCH = main.ino

UPLOAD_PORT = /dev/ttyUSB0
UPLOAD_SPEED = 921600
BOARD = esp32doit-devkit-v1

BUILD_DIR = $(PWD)/build

FS_DIR = $(PWD)/fsroot

ESP_LIBS=$(shell echo $HOME)/.arduino15/packages/esp32/hardware/esp32/1.0.4/libraries

# RTC_DS1307 = true # Enable RTC support
RTC_DS1307 =

# Versioning
GIT_DESC=$(shell git describe --tags --long)
WSVERSION=$(GIT_DESC)

BUILD_EXTRA_FLAGS += -I./include -Wall -DWSTATION_VERSION=\"${WSVERSION}\" -DCORE_DEBUG_LEVEL=5

LIBS=$(ESP_LIBS)/Wire \
	 $(ESP_LIBS)/SPI \
	 $(ESP_LIBS)/WiFi \
	 $(ESP_LIBS)/WiFiClientSecure \
	 $(ESP_LIBS)/HTTPClient \
	 $(ESP_LIBS)/FS \
	 $(ESP_LIBS)/SPIFFS \
	 libs/AsyncTCP \
	 libs/ESPAsyncWebServer \
	 libs/ArduinoJson-6.15.1/ \
	 libs/Adafruit_GFX_Library-1.7.5 \
	 libs/Adafruit_ILI9341-1.5.4 \
	 libs/Time \
	 libs/DHTesp \
	 libs/ArduinoNvs

ifeq ($(RTC_DS1307),true)
LIBS += libs/DS1307RTC
BUILD_EXTRA_FLAGS += -DRTC_DS1307
endif

PNGDIR=../resources/icons/png

.PHONY: png clean-png

include makeEspArduino.mk

# Convert PNG files to .c RGB565
png:
	@mkdir -p $(FS_DIR)
	@$(PNGDIR)/convert_to_file.py $(PNGDIR) $(FS_DIR)

clean-png:
	@rm -rf $(PNGCDIR)
