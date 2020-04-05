SKETCH = main.ino

UPLOAD_PORT = /dev/ttyUSB0
UPLOAD_SPEED = 921600
BOARD = esp32doit-devkit-v1

BUILD_DIR = $(PWD)/build

ESP_LIBS=$(shell echo $HOME)/.arduino15/packages/esp32/hardware/esp32/1.0.4/libraries

LIBS=$(ESP_LIBS)/SPI \
	 $(ESP_LIBS)/WiFi \
	 $(ESP_LIBS)/WebServer \
	 $(ESP_LIBS)/HTTPClient \
	 libs/Adafruit_GFX_Library-1.7.5 \
	 libs/Adafruit_ILI9341-1.5.4 \
	 png

PNGDIR=../resources/icons/png
PNGCDIR=$(PWD)/png

.PHONY: png clean-png

include makeEspArduino.mk

# Convert PNG files to .c RGB565
png:
	@mkdir -p $(PNGCDIR)
	@$(PNGDIR)/convert_to_C.py $(PNGDIR) $(PNGCDIR)

clean-png:
	@rm -rf $(PNGCDIR)
