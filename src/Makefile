SKETCH = main.ino

UPLOAD_PORT = /dev/ttyUSB0
UPLOAD_SPEED = 921600
BOARD = esp32doit-devkit-v1

BUILD_DIR = $(PWD)/build

FS_DIR = $(PWD)/fsroot

ESP_LIBS=$(shell echo $HOME)/.arduino15/packages/esp32/hardware/esp32/1.0.4/libraries

BUILD_EXTRA_FLAGS += -I./include

LIBS=$(ESP_LIBS)/Wire \
	 $(ESP_LIBS)/SPI \
	 $(ESP_LIBS)/WiFi \
	 $(ESP_LIBS)/WebServer \
	 $(ESP_LIBS)/HTTPClient \
	 $(ESP_LIBS)/FS \
	 $(ESP_LIBS)/SPIFFS \
	 libs/Adafruit_GFX_Library-1.7.5 \
	 libs/Adafruit_ILI9341-1.5.4 \
	 libs/Time \
	 libs/DS1307RTC

PNGDIR=../resources/icons/png

.PHONY: png clean-png

include makeEspArduino.mk

# Convert PNG files to .c RGB565
png:
	@mkdir -p $(FS_DIR)
	@$(PNGDIR)/convert_to_file.py $(PNGDIR) $(FS_DIR)

clean-png:
	@rm -rf $(PNGCDIR)
