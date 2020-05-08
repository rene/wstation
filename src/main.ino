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
EInterface *gui = NULL;
/** Color theme */
ETheme colorTheme;
/** OpenWeather */
OpenWeather weatherWS;
/** WiFi */
WiFiMulti wifiMulti;

/** Mutex for update screen */
volatile SemaphoreHandle_t t_mutex;

unsigned long lastUpdate;


/**
 * \brief Update graphical elements on the screen
 */
void taskUpdateScreen(void *parameter)
{
	tmElements_t tm;

	while(1) {
		// Update clock
		if (!RTC.read(tm)) {
			Serial.println("DS1307 read error!");
			if (RTC.chipPresent()) {
				tm.Day    = 1;
				tm.Month  = 1;
				tm.Year   = CalendarYrToTm(2020);
				tm.Hour   = 0;
				tm.Minute = 0;
				tm.Second = 0;
				RTC.write(tm);
				Serial.println("DS1307 was stopped. Time was reset.");
			}
		} else {
			xSemaphoreTake(t_mutex, portMAX_DELAY);
			if (gui) {
				gui->setHours(tm.Hour);
				gui->setMinutes(tm.Minute);
				gui->setSeconds(tm.Second);
			}
			xSemaphoreGive(t_mutex);
		}

		delay(1000);
	}
}

/**
 * \brief Setup
 */
void setup() {
	// Initialize serial
    Serial.begin(115200);
	Serial.print("WSTATION: ");
	Serial.println(WSTATION_VERSION);

	// Initialize general purpose LED
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	// Initialize SPIFFS file system
	if(!SPIFFS.begin(false)){
		Serial.println("SPIFFS Mount Failed");
		Serial.println("Abort.");
        return;
    }
	delay(500);

	// Initialize semaphore
	vSemaphoreCreateBinary(t_mutex);

	// Initialize embedded GUI
	gui = new EInterface(TFT_CS, TFT_DC,
			TFT_BACKLIGHT, BACKLIGHT_DEFAULT, colorTheme,
			&SPIFFS);

	gui->initialize();
	gui->showLogo();
	delay(700);

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

	xTaskCreate(taskUpdateScreen, "UpdateScreen", 10000, NULL, 2, NULL);
#endif
}

/**
 * \brief Main loop
 */
void loop()
{
	xSemaphoreTake(t_mutex, portMAX_DELAY);
#if 1
	gui->showTemp1(20 + (0.1 * random(9)));
	gui->showHumidity1(random(110));
#endif
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
				gui->showTemp2(t1);
				gui->showWeather(w.weather, 0);
				gui->showHumidity2(w.humidity);
			}
		}
	} else {
		gui->showWiFi(false);
		gui->setIP("");
	}

	xSemaphoreGive(t_mutex);
	delay(1200);
}

