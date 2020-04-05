#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "png_list.h"

#define TFT_DC 16
#define TFT_CS 17

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
	pinMode(22, OUTPUT);
	digitalWrite(22, HIGH);
  
    Serial.begin(115200);
	Serial.println("Hello World!");

	tft.begin();
	tft.setRotation(2);
	tft.fillScreen(ILI9341_BLACK);

	tft.drawRGBBitmap(0, 0, _png_01d,
		_png_01d_width, _png_01d_height);

	tft.drawRGBBitmap(0, 61, _png_02d,
		_png_02d_width, _png_02d_height);

	tft.drawRGBBitmap(0, 122, _png_10n,
		_png_10n_width, _png_10n_height);

	tft.drawRGBBitmap(0, 183, _png_11d,
		_png_11d_width, _png_11d_height);
}

void loop()
{
	Serial.println("Ola");
	delay(1000);
}

