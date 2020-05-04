#include <WiFi.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <DS1307RTC.h>
#include "wstation.h"
#include "ETheme.h"
#include "EInterface.h"
#include "OpenWeather.h"

#define PRINT2DIG(out, num) do { \
		if(num >= 0 && num < 10) \
			out.write('0'); \
		out.print(num); \
	} while(0)

/** Embedded GUI */
EInterface *gui;
/** Color theme */
ETheme colorTheme;
/** OpenWeather */
OpenWeather weatherWS;
/** WiFi */
WiFiMulti wifiMulti;

unsigned long lastUpdate;

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

	lastUpdate = now() - 70;

	wifiMulti.addAP("VIZSLANET", "teodoro+2015");

	weatherWS.setAPIKey("0300414e2812dcf9b846060f08ae4882");
	weatherWS.setCity("Berlin,DE");
#if 0
	OpenWeather weatherWS("0300414e2812dcf9b846060f08ae4882");
	weatherWS.setCity("Berlin,DE");

	wifiMulti.addAP("VIZSLANET", "teodoro+2015");

	gui->print(10, 60, "Connecting...");
	while (wifiMulti.run() != WL_CONNECTED) {
		delay(500);
	}
	
	gui->print(10, 60, "Connected!");
	
	char ip[40]; IPAddress ipAddr = WiFi.localIP();
	snprintf(ip, 40, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	gui->print(0, 210, ip);

	if (weatherWS.updateDailyForecast() < 0) {
		gui->print("ERROR!\n");
	}
#endif
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
//	gui->setIP("192.168.200.120");
//	gui->showWiFi(true);
//	gui->showTemp1(20.1);
//	gui->showTemp2(-2.0);
//	gui->showHumidity1(55);
	gui->showHumidity2(80);
	gui->showChannel(1);

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
#if 1
	tmElements_t tm;

	if (!RTC.read(tm)) {
		//Serial.print(tmYearToCalendar(tm.Year));
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

	gui->showTemp2(20 + (0.1 * random(9)));
	gui->showHumidity2(random(110));
	gui->setHours(tm.Hour);
	gui->setMinutes(tm.Minute);
	gui->setSeconds(tm.Second);
#endif
	delay(1000);

	if (wifiMulti.run() == WL_CONNECTED) {
		gui->showWiFi(true);
		char ip[40]; IPAddress ipAddr = WiFi.localIP();
		snprintf(ip, 40, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
		gui->setIP(ip);

		if ((now() - lastUpdate) >= 60) {
			lastUpdate = now();

			if (weatherWS.updateDailyForecast() < 0) {
				Serial.print("Error");
			} else {
				weather_info_t w = weatherWS.getDailyForecast();
				float t1 = OpenWeather::convKelvinTemp(w.temp, CELSIUS);
				gui->showTemp1(t1);
				gui->showWeather(w.weather, 0);
				gui->showHumidity1(w.humidity);
			}
		}
	} else {
		gui->showWiFi(false);
		gui->setIP("");
	}
}

