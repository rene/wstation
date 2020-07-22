/* SPDX-License-Identifier: BSD-3-Clause */
/* 
 * Copyright 2020 Renê de Souza Pinto
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file main.ino
 * \brief Main tasks, setup() and loop() functions
 */
#include <WiFi.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <TimeLib.h>
#include <DHTesp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wstation.h"
#include "clock.h"
#include "nexus.h"
#include "ETheme.h"
#include "EInterface.h"
#include "OpenWeather.h"
#include "UserConf.h"

/** User configuration data */
UserConf confData;
/** Embedded GUI */
EInterface *gui = NULL;
/** Color theme */
ETheme colorTheme;
/** OpenWeather */
OpenWeather weatherWS;
/** WiFi */
WiFiMulti wifiMulti;
/** DHT sensor */
DHTesp dhtSensor;
/** Wall clock */
tmElements_t wallClock;
/** Web server */
AsyncWebServer webServer(WEBSERVER_PORT);

/** Mutex for update screen */
volatile SemaphoreHandle_t t_mutex;

/** Last weather information update */
unsigned long lastUpdate;

/** Last NTP date/time update */
unsigned long lastNTPUpdate;

/**
 * \brief Format city string
 * \param [in] city City
 * \return String
 */
String formatCity(const String& city)
{
	int cpos = city.indexOf(",");
	if (cpos > 0) {
		return city.substring(0, cpos);
	} else {
		return city;
	}
}

/**
 * \brief Format date into string
 * \param [in] tm Time
 * \return String
 */
String formatDate(tmElements_t tm)
{
	String dateStr;
	dateStr = String(dayStr(tm.Wday)).substring(0,3) + ", "
				+ String(monthStr(tm.Month)).substring(0,3) + " "
				+ String(tm.Day) + ", "
				+ String(tmYearToCalendar(tm.Year));
	return dateStr;
}

/**
 * \brief Format IP address into string
 * \param [in] ipAddr IP Address
 * \return String
 */
String formatIP(IPAddress ipAddr)
{
	char ip[40];
	snprintf(ip, 40, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	return String(ip);
}

/**
 * \brief Update graphical elements on the screen
 * \param parameter Task parameters (not used)
 */
void taskUpdateScreen(void *parameter)
{
	bool updateStrDate = true;
	while(1) {
		// Update clock
		if (readClock(&wallClock) < 0) {
			Serial.println("Read clock error!");
			wallClock.Day    = 1;
			wallClock.Month  = 1;
			wallClock.Year   = CalendarYrToTm(2020);
			wallClock.Hour   = 0;
			wallClock.Minute = 0;
			wallClock.Second = 0;
			writeClock(&wallClock);
			updateStrDate = true;
		} else {
			if (wallClock.Hour == 0 && wallClock.Minute == 0) {
				updateStrDate = true;
			}

			xSemaphoreTake(t_mutex, portMAX_DELAY);
			if (gui) {
				gui->setHours(wallClock.Hour);
				gui->setMinutes(wallClock.Minute);
				gui->setSeconds(wallClock.Second);
				if (updateStrDate) {
					gui->setDate(formatDate(wallClock));
					updateStrDate = false;
				}
			}
			xSemaphoreGive(t_mutex);
		}
		delay(1000);
	}
}

/**
 * \brief Update forecast information on the screen
 * \param parameter Task parameters (not used)
 */
void taskUpdateWeatherInfo(void *parameter)
{
	int i;
	float t1, tf1, tf2;
	weather_info_t wfc;

	while (1) {
		if (wifiMulti.run() == WL_CONNECTED) {
			if ((now() - lastUpdate) >= WEATHER_UPDATE_INTERVAL) {
				lastUpdate = now();

				if (weatherWS.updateForecast() < 0) {
					Serial.print("Error");
				} else {
					weather_info_t w = weatherWS.getDailyForecast();
					t1 = OpenWeather::convKelvinTemp(w.temp, CELSIUS);

					xSemaphoreTake(t_mutex, portMAX_DELAY);
					gui->showWeather(w.weather, 0);

					for (i = 0; i < 3; i++) {
						wfc = weatherWS.getWeeklyForecast(i);
						tf1 = OpenWeather::convKelvinTemp(wfc.feels, CELSIUS);
						tf2 = OpenWeather::convKelvinTemp(wfc.temp, CELSIUS);

						gui->showForecastLabel(i, dayShortStr(weekday(wfc.date)));
						gui->showForecastTemp1(i, tf1);
						gui->showForecastTemp2(i, tf2);
						gui->showForecastWeather(i, wfc.weather);
					}
					xSemaphoreGive(t_mutex);
				}
			}
		}
		delay(1000);
	}
}

/**
 * \brief Update NTP date/time information
 * \param parameter Task parameters (not used)
 */
void taskUpdateNTP(void *parameter)
{
	int i;
	float t1, tf1, tf2;
	weather_info_t wfc;

	while (1) {
		if (wifiMulti.run() == WL_CONNECTED) {
			if ((now() - lastNTPUpdate) >= NTP_UPDATE_INTERVAL) {
				lastNTPUpdate = now();

				// NTP update
				configTime(confData.getTimezone(), confData.getDaylight(),
					confData.getNTPServer().c_str());
				// TODO update wallclock

				// Update date on screen (time will be updated on the next
				// second)
				xSemaphoreTake(t_mutex, portMAX_DELAY);
				gui->setDate(formatDate(wallClock));
				xSemaphoreGive(t_mutex);
			}
		}
		delay(1000);
	}
}

/**
 * \brief Read DHT sensor data
 * \param parameter Task parameters (not used)
 */
void taskReadDHTSensor(void *parameter)
{
	TempAndHumidity sensorData;
	while (1) {
		sensorData = dhtSensor.getTempAndHumidity();

		if (dhtSensor.getStatus() != 0) {
			Serial.println("DHT11 error status: " + String(dhtSensor.getStatusString()));
		} else {
			// Display data
			xSemaphoreTake(t_mutex, portMAX_DELAY);
			gui->showTemp1(sensorData.temperature);
			gui->showHumidity1(sensorData.humidity);
			xSemaphoreGive(t_mutex);
		}

		delay(10000);
	}
}

/**
 * \brief Receive nexus sensor data
 * \param parameter Task parameters (not used)
 */
void taskReceiveSensorData(void *parameter)
{
	int i, ch, disp;
	nexus_t sensors[3];
	unsigned long lastShow, lastData[3];
	float t;

	// Invalidate initial data
	for (i = 0; i < 3; i++) {
		sensors[i].flags.fields.channel = NEXUS_INVALID_CHANNEL;
	}

	i        = 0;
	disp     = 0;
	lastShow = now();
	while (1) {
		// Check if we have received data
		if (nexusDataAvailable) {
			portENTER_CRITICAL(&nexusMutex);

			nexusDataAvailable = false;

			// Read channel
			ch = nexusData.flags.fields.channel;
			if (ch >= NEXUS_CHANNEL_1 && ch <= NEXUS_CHANNEL_3) {
				i = ch;
			} else {
				// Invalid channel
				portEXIT_CRITICAL(&nexusMutex);
				continue;
			}

			// Copy data
			sensors[i].id          = nexusData.id;
			sensors[i].flags.raw   = nexusData.flags.raw;
			sensors[i].temperature = nexusData.temperature;
			sensors[i].humidity    = nexusData.humidity;
			lastData[i]            = now();

			portEXIT_CRITICAL(&nexusMutex);

			// Indicate on screen that data has been received
			xSemaphoreTake(t_mutex, portMAX_DELAY);
			gui->showRadio(true);
			delay(300);
			gui->showRadio(false);
			xSemaphoreGive(t_mutex);
		}

		// Show data on the screen
		if ((now() - lastShow) >= SENSOR_DISPLAY_INTERVAL) {
			if (sensors[disp].flags.fields.channel != NEXUS_INVALID_CHANNEL) {
				// Check for how long sensor data has not been updated
				lastShow = now();
				if ((lastShow - lastData[disp]) <= SENSOR_DATA_EXPIRATION) {
					t = (float)sensors[disp].temperature / 10;
					xSemaphoreTake(t_mutex, portMAX_DELAY);
					gui->showChannel(sensors[disp].flags.fields.channel + 1);
					gui->showTemp2(t);
					gui->showHumidity2(sensors[disp].humidity);
					xSemaphoreGive(t_mutex);
				} else {
					// Sensor data is expired, let's invalidate it
					sensors[disp].flags.fields.channel = NEXUS_INVALID_CHANNEL;
					xSemaphoreTake(t_mutex, portMAX_DELAY);
					gui->showChannel(GUI_INV_CHANNEL);
					gui->showTemp2(GUI_INV_TEMP);
					gui->showHumidity2(GUI_INV_HUMIDITY);
					xSemaphoreGive(t_mutex);
				}
			}

			disp++;
			if (disp >= 3) {
				disp = 0;
			}
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
	gui->showVersion(50, 200);
	delay(700);

	// Initialize DHT sensor
	dhtSensor.setup(DHT_DATA_PIN, DHTesp::AUTO_DETECT);

	// Initialize 433MHz module receiver
	setupNexus(RF_PIN);

	// Configure web server
	webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "text/html", "<html><center><img src='/logo.png'/></center><p>Ola mundo!</p></html>");
	});
	webServer.serveStatic("/logo.png", SPIFFS, "/logo.png");

	// Read user configuration
	confData.ReadConf();

	// Check for valid configuration
	if (!confData.isConfigured()) {
		// Reset conf
		confData.ResetConf();
#if 0
	gui->print(30, 25, "Welcome to WStation!");
	gui->print(10, 60, "Device needs configuration!");
	gui->print(0, 210, "Please, connect to:\n\n");
	gui->print("  ESSID:    WStation\n");
	gui->print("  Password: wstation1234\n");
#endif
	}

	// DEMO
	lastUpdate    = now() - WEATHER_UPDATE_INTERVAL;
	lastNTPUpdate = now() - NTP_UPDATE_INTERVAL + 5;

	wifiMulti.addAP("VIZSLANET", "teodoro+2015");

	weatherWS.setAPIKey("0300414e2812dcf9b846060f08ae4882");
	weatherWS.setCity("Berlin,DE");

#if 1
	gui->clearAll();
	gui->showAll();

	gui->setCity(formatCity(weatherWS.getCity()));

	xTaskCreate(taskUpdateScreen,      "UpdateScreen",      10240, NULL, 2, NULL);
	xTaskCreate(taskReceiveSensorData, "ReceiveSensorData", 10240, NULL, 0, NULL);
	xTaskCreate(taskUpdateWeatherInfo, "UpdateWeatherInfo", 32768, NULL, 0, NULL);
	xTaskCreate(taskUpdateNTP,         "UpdateNTP",          4096, NULL, 0, NULL);
	xTaskCreate(taskReadDHTSensor,     "ReadDHTSensor",      4096, NULL, 0, NULL);
#endif
}

/**
 * \brief Main loop
 */
void loop()
{
	int status  = WL_IDLE_STATUS;
	bool wsinit = false;
	bool icon   = false;

	while (1) {
		// WiFi status
		status = wifiMulti.run();

		switch(status) {
			case WL_IDLE_STATUS:
			case WL_NO_SSID_AVAIL:
				do {
					status = wifiMulti.run();

					if (icon == false)
						icon = true;
					else
						icon = false;

					xSemaphoreTake(t_mutex, portMAX_DELAY);
					gui->showWiFi(icon);
					gui->setIP("");
					xSemaphoreGive(t_mutex);
					delay(1000);
				} while (status == WL_IDLE_STATUS ||
						 status == WL_NO_SSID_AVAIL);
				break;

			case WL_CONNECTED:
				xSemaphoreTake(t_mutex, portMAX_DELAY);
				gui->showWiFi(true);
				gui->setIP(formatIP(WiFi.localIP()));
				xSemaphoreGive(t_mutex);

				if (!wsinit) {
					wsinit = true;
					webServer.begin();
				}
				break;

			case WL_SCAN_COMPLETED:
			case WL_NO_SHIELD:
			case WL_CONNECT_FAILED:
			case WL_CONNECTION_LOST:
			case WL_DISCONNECTED:
			default:
				xSemaphoreTake(t_mutex, portMAX_DELAY);
				gui->showWiFi(false);
				gui->setIP("");
				xSemaphoreGive(t_mutex);
				break;
		}

		delay(1000);
	}
}

