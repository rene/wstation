#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <DS1307RTC.h>
#include "wstation.h"
#include "ETheme.h"
#include "EInterface.h"

#define PRINT2DIG(out, num) do { \
		if(num >= 0 && num < 10) \
			out.write('0'); \
		out.print(num); \
	} while(0)

/** Embedded GUI */
EInterface *gui;
/** Color theme */
ETheme colorTheme;

void setup() {
	// Initialize serial
    Serial.begin(115200);
	Serial.print("WSTATION: ");
	Serial.println(WSTATION_VERSION);

	// Initialize SPIFFS file system
	if(!SPIFFS.begin(false)){
		Serial.println("SPIFFS Mount Failed");
		Serial.println("Abort.");
        return;
    }
	delay(500);

	// Initialize embedded GUI
	gui = new EInterface(TFT_CS, TFT_DC,
			TFT_BACKLIGHT, BACKLIGHT_DEFAULT, colorTheme,
			&SPIFFS);

	gui->initialize();
	gui->showAll();

	gui->setCity("Berlin");
	gui->setIP("192.168.200.120");
	gui->showWiFi(true);
	gui->showTemp1(20.1);
	gui->showTemp2(-2.0);
	gui->showHumidity1(55);
	gui->showHumidity2(80);
#if 0
	tft.setFont(&FreeSans9pt7b);
	tft.setCursor(70,55);
	tft.print("13 de Abril de 2020");

	// Forecast
	draw_pixmap_half(5, 240, "/01d.px");
	tft.setFont(&FreeSans9pt7b);
	tft.setTextColor(0x8410); // RGB(128,128,128)
	tft.setCursor(40, 260);
	tft.print("Seg");

	tft.setCursor(5, 285);
	tft.setTextColor(0x7D1F); // RGB(120, 160, 255)
	tft.print("22.3 C");

	tft.setCursor(5, 300);
	tft.setTextColor(0xF186); // RGB(240, 40, 40)
	tft.print("18.0 C");

	draw_pixmap_half(90, 240, "/10d.px");
	tft.setFont(&FreeSans9pt7b);
	tft.setTextColor(0x8410); // RGB(128,128,128)
	tft.setCursor(125, 260);
	tft.print("Ter");

	tft.setCursor(90, 285);
	tft.setTextColor(0x7D1F); // RGB(120, 160, 255)
	tft.print("20.3 C");

	tft.setCursor(90, 300);
	tft.setTextColor(0xF186); // RGB(240, 40, 40)
	tft.print("15.0 C");

	draw_pixmap_half(168, 240, "/11d.px");
	tft.setFont(&FreeSans9pt7b);
	tft.setTextColor(0x8410); // RGB(128,128,128)
	tft.setCursor(203, 260);
	tft.print("Qua");

	tft.setCursor(168, 285);
	tft.setTextColor(0x7D1F); // RGB(120, 160, 255)
	tft.print("15.1 C");

	tft.setCursor(168, 300);
	tft.setTextColor(0xF186); // RGB(240, 40, 40)
	tft.print("9.0 C");
#endif
}

void loop()
{
	tmElements_t tm;

	if (RTC.read(tm)) {
		Serial.print("Ok, Time = ");
		PRINT2DIG(Serial, tm.Hour);
		Serial.write(':');
		PRINT2DIG(Serial, tm.Minute);
		Serial.write(':');
		PRINT2DIG(Serial, tm.Second);
		Serial.print(", Date (D/M/Y) = ");
		Serial.print(tm.Day);
		Serial.write('/');
		Serial.print(tm.Month);
		Serial.write('/');
		Serial.print(tmYearToCalendar(tm.Year));
		Serial.println();
	} else {
		if (RTC.chipPresent()) {
			tm.Day    = 1;
			tm.Month  = 1;
			tm.Year   = CalendarYrToTm(2020);
			tm.Hour   = 0;
			tm.Minute = 0;
			tm.Second = 0;
			RTC.write(tm);
			Serial.println("DS1307 was stopped. Time was reset.");
		} else {
			Serial.println("DS1307 read error!  Please check the circuitry.");
			Serial.println();
		}
		delay(9000);
	}
	delay(1000);

	gui->showTemp1(20 + (0.1 * random(9)));
	gui->showHumidity2(random(99));
	gui->setHours(tm.Hour);
	gui->setMinutes(tm.Minute);
	gui->setSeconds(tm.Second);
}

