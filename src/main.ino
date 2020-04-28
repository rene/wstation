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
	gui->showLogo();
	delay(1000);

#if 0
	gui->print(30, 25, "Welcome to WStation!");
	gui->print(10, 60, "Device needs configuration!");
	gui->print(0, 210, "Please, connect to:\n\n");
	gui->print("  ESSID:    WStation\n");
	gui->print("  Password: wstation1234\n");
#endif
#if 1
	gui->clearAll();
	gui->showAll();

	gui->setCity("Berlin");
	gui->setDate("April 13, 2020");
	gui->setIP("192.168.200.120");
	gui->showWiFi(true);
	gui->showTemp1(20.1);
	gui->showTemp2(-2.0);
	gui->showHumidity1(55);
	gui->showHumidity2(80);
	gui->showChannel(2);

	gui->showForecastWeather(0, CLEAR_SKY);
	gui->showForecastWeather(1, SNOW_SHOWER);
	gui->showForecastWeather(2, RAIN_LIGHT);
	gui->showForecastLabel(0, "Mon");
	gui->showForecastLabel(1, "Thu");
	gui->showForecastLabel(2, "Wed");
	gui->showForecastTemp1(0, 18.0);
	gui->showForecastTemp2(0, 22.3);
	gui->showForecastTemp1(1, 20.3);
	gui->showForecastTemp2(1, 15.0);
	gui->showForecastTemp1(2, -2.3);
	gui->showForecastTemp2(2, 9.0);
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

#if 1
	gui->showTemp1(20 + (0.1 * random(9)));
	gui->showHumidity2(random(110));
	gui->setHours(tm.Hour);
	gui->setMinutes(tm.Minute);
	gui->setSeconds(tm.Second);
#endif
}

