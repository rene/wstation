#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "png_list.h"

#define TFT_DC 16
#define TFT_CS 17

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
	/* PWM Channel 0: Pin 22 - Backlight */
	ledcSetup(0, 5000, 9);
	ledcAttachPin(22, 0);
	ledcWrite(0, 200);

    Serial.begin(115200);
	Serial.println("Hello World!");

	tft.begin();
	tft.setRotation(2);
	tft.fillScreen(ILI9341_BLACK);

	tft.drawRGBBitmap(0, 0, _png_01d,
		_png_01d_width, _png_01d_height);

	tft.drawRGBBitmap(61, 0, _png_01n,
		_png_01n_width, _png_01n_height);

	tft.drawRGBBitmap(123, 0, _png_02d,
		_png_02d_width, _png_02d_height);

	tft.drawRGBBitmap(0, 61, _png_02n,
		_png_02n_width, _png_02n_height);

	tft.drawRGBBitmap(61, 61, _png_03d,
		_png_03d_width, _png_03d_height);

	tft.drawRGBBitmap(123, 61, _png_04d,
		_png_04d_width, _png_04d_height);

	tft.drawRGBBitmap(0, 123, _png_09d,
		_png_09d_width, _png_09d_height);

	tft.drawRGBBitmap(61, 123, _png_10d,
		_png_10d_width, _png_10d_height);

	tft.drawRGBBitmap(123, 123, _png_10n,
		_png_10n_width, _png_10n_height);


	tft.drawRGBBitmap(0, 184, _png_13n,
		_png_13n_width, _png_13n_height);

	tft.drawRGBBitmap(61, 184, _png_50d,
		_png_50d_width, _png_50d_height);

	tft.drawRGBBitmap(123, 195, _png_battery,
		_png_battery_width, _png_battery_height);


}

void loop()
{
	Serial.println("Ola");
	delay(1000);
}

