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
 * @file main.ino
 * Main tasks, setup(), loop() and auxiliary functions
 */
#include <WiFi.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <TimeLib.h>
#include <DHTesp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoNvs.h>
#include "wstation.h"
#include "clock.h"
#include "nexus.h"
#include "ETheme.h"
#include "EInterface.h"
#include "OpenWeather.h"
#include "UserConf.h"
#include "webservices.cpp"

/** User configuration data */
UserConf confData;
/** Embedded GUI */
EInterface *gui = NULL;
/** Color theme */
ETheme colorTheme;
/** OpenWeather */
OpenWeather weatherWS;
/** DHT sensor */
DHTesp dhtSensor;
/** Wall clock */
tmElements_t wallClock;
/** Web server */
AsyncWebServer webServer(WEBSERVER_PORT);

/** Mutex for screen update */
volatile SemaphoreHandle_t t_mutex;

/** Mutex for wall clock update */
volatile SemaphoreHandle_t clk_mutex;

/** Mutex for restart device */
volatile SemaphoreHandle_t reset_mutex;

/** Wait for user initial setup */
volatile SemaphoreHandle_t setup_sem;

/** Last weather information update */
unsigned long lastUpdate;

/** Last NTP date/time update */
unsigned long lastNTPUpdate;

/** Update the date on main screen */
bool updateStrDate;


/**
 * Format city string
 * @param [in] city City
 * @return String
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
 * Format date into string
 * @param [in] tm Time
 * @return String
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
 * Format IP address into string
 * @param [in] ipAddr IP Address
 * @return String
 */
String formatIP(IPAddress ipAddr)
{
	char ip[40];
	snprintf(ip, 40, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	return String(ip);
}

/**
 * Indicate that user setup is done
 */
void userSetupDone(void)
{
	xSemaphoreGive(setup_sem);
}

/**
 * Update screen elements from user configuration
 */
void updateFromConf(void)
{
	int d, m, y, w;

	// Update calendar, daylight and timezone values are used only for NTP
	// server (in order to perform the right time shift)
	xSemaphoreTake(clk_mutex, portMAX_DELAY);
	confData.getDate(&d, &m, &y, &w);

	wallClock.Day    = d;
	wallClock.Month  = m;
	wallClock.Year   = CalendarYrToTm(y);

	wallClock.Hour   = confData.getHours();
	wallClock.Minute = confData.getMinutes();
	wallClock.Second = confData.getSeconds();
	writeClock(&wallClock);
	xSemaphoreGive(clk_mutex);

	xSemaphoreTake(t_mutex, portMAX_DELAY);
	// LCD backlight
	gui->setBacklight(confData.getLCDBrightness());
	// Temperature scale
	gui->setTempScale(confData.getTempScale());
	xSemaphoreGive(t_mutex);

	// Set to update date string
	xSemaphoreTake(t_mutex, portMAX_DELAY);
	updateStrDate = true;
	xSemaphoreGive(t_mutex);
}

/**
 * Update graphical elements on the screen
 * @param parameter Task parameters (not used)
 */
void taskUpdateScreen(void *parameter)
{
	int ret;
	updateStrDate = true;
	while(1) {
		// Update clock
		xSemaphoreTake(clk_mutex, portMAX_DELAY);
		ret = readClock(&wallClock);
		xSemaphoreGive(clk_mutex);
		if (ret < 0) {
			log_e("Read clock error!");
			xSemaphoreTake(clk_mutex, portMAX_DELAY);
			wallClock.Day    = 1;
			wallClock.Month  = 1;
			wallClock.Year   = CalendarYrToTm(2020);
			wallClock.Hour   = 0;
			wallClock.Minute = 0;
			wallClock.Second = 0;
			writeClock(&wallClock);
			xSemaphoreGive(clk_mutex);

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
 * Update forecast information on the screen
 * @param parameter Task parameters (not used)
 */
void taskUpdateWeatherInfo(void *parameter)
{
	int i;
	float t1, tf1, tf2;
	weather_info_t wfc;

	while (1) {
		if (WiFi.status() == WL_CONNECTED) {
			if ((now() - lastUpdate) >= WEATHER_UPDATE_INTERVAL) {
				lastUpdate = now();

				if (weatherWS.updateForecast() != 0) {
					log_e("Error to retrieve forecast!");
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
 * Update NTP date/time information
 * @param parameter Task parameters (not used)
 */
void taskUpdateNTP(void *parameter)
{
	int i;
	float t1, tf1, tf2;
	weather_info_t wfc;

	while (1) {
		if (WiFi.status() == WL_CONNECTED) {
			if ((now() - lastNTPUpdate) >= NTP_UPDATE_INTERVAL) {
				lastNTPUpdate = now();

				// NTP update
				xSemaphoreTake(clk_mutex, portMAX_DELAY);
				configTime(confData.getTimezone(), confData.getDaylight(),
					confData.getNTPServer().c_str());
				// Update wall clock
				getSysClock(&wallClock);
				xSemaphoreGive(clk_mutex);

				// Update date on screen (time will be updated on the next
				// second)
				xSemaphoreTake(t_mutex, portMAX_DELAY);
				updateStrDate = true;
				xSemaphoreGive(t_mutex);
			}
		}
		delay(1000);
	}
}

/**
 * Read DHT sensor data
 * @param parameter Task parameters (not used)
 */
void taskReadDHTSensor(void *parameter)
{
	TempAndHumidity sensorData;
	while (1) {
		sensorData = dhtSensor.getTempAndHumidity();

		if (dhtSensor.getStatus() != 0) {
			log_e("DHT11 error status: %s", dhtSensor.getStatusString());
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
 * Receive nexus sensor data
 * @param parameter Task parameters (not used)
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
 * Setup
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
		log_e("SPIFFS Mount Failed");
		log_e("Abort.");
		return;
    }

	// Initialize NVS
	NVS.begin();

	delay(500);

	// Initialize semaphores
	vSemaphoreCreateBinary(t_mutex);
	vSemaphoreCreateBinary(clk_mutex);
	vSemaphoreCreateBinary(reset_mutex);
	setup_sem = xSemaphoreCreateCounting(1, 0);

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

	// Read user configuration
	confData.ReadConf();

	// Configure web server
	SetupWebServices(&webServer);

	// Check for valid configuration
	if (!confData.isConfigured()) {
		// Reset conf
		confData.ResetConf();

		// Create Access Point
		WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS);
		String ip("  URL: http://");
		ip.concat(formatIP(WiFi.softAPIP()));

		// Start web server
		webServer.begin();

		// Show instructions on screen
		gui->print(30, 25, "Welcome to WStation!");
		gui->print(10, 60, "Device needs configuration!");
		gui->print(0, 224, "Please, connect to:\n");
		gui->print("  ESSID:    " DEFAULT_AP_SSID "\n");
		gui->print("  Password: " DEFAULT_AP_PASS "\n");
		gui->print(ip);

		// Wait until setup is done
		xSemaphoreTake(setup_sem, portMAX_DELAY);
	}

	// Device is configured, proceed with initialization
	lastUpdate    = now() - WEATHER_UPDATE_INTERVAL;
	lastNTPUpdate = now() - NTP_UPDATE_INTERVAL + 5;

	WiFi.mode(WIFI_STA);
	WiFi.begin(confData.getWiFiSSID().c_str(),
				confData.getWiFiPassword().c_str());
	weatherWS.setAPIKey(confData.getAPIKey());
	weatherWS.setCity(confData.getCity());

	gui->clearAll();
	gui->showAll();
	gui->setCity(formatCity(weatherWS.getCity()));

	updateFromConf();

	xTaskCreate(taskUpdateScreen,      "UpdateScreen",      16384, NULL, 2, NULL);
	xTaskCreate(taskReceiveSensorData, "ReceiveSensorData", 16384, NULL, 0, NULL);
	xTaskCreate(taskUpdateWeatherInfo, "UpdateWeatherInfo", 36864, NULL, 0, NULL);
	xTaskCreate(taskUpdateNTP,         "UpdateNTP",          8192, NULL, 0, NULL);
	xTaskCreate(taskReadDHTSensor,     "ReadDHTSensor",      4096, NULL, 0, NULL);
}

/**
 * Main loop
 */
void loop()
{
	int status     = WL_IDLE_STATUS;
	bool wsinit    = false;
	bool icon      = false;
	int nocontimer = 0;
	int i;

	while (1) {
		// WiFi status
		status = WiFi.status();

		switch(status) {
			case WL_IDLE_STATUS:
			case WL_NO_SSID_AVAIL:
				do {
					if (nocontimer < NETWORK_CONN_RETRY)
						nocontimer++;
					else
						break;

					status = WiFi.status();

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
				nocontimer = 0;
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
				nocontimer++;
				break;
		}

		// Check for connection retry
		if (nocontimer >= NETWORK_CONN_RETRY) {
			// Try to reconnect
			WiFi.disconnect();
			delay(1000);
			WiFi.mode(WIFI_STA);
			WiFi.begin(confData.getWiFiSSID().c_str(),
						confData.getWiFiPassword().c_str());

			// Quick blink WiFi icon
			xSemaphoreTake(t_mutex, portMAX_DELAY);
			for (i = 0; i < 4; i++) {
				if ((i % 2) == 0)
					gui->showWiFi(true);
				else
					gui->showWiFi(false);
				delay(400);
			}
			xSemaphoreGive(t_mutex);

			// Reset timer counter
			nocontimer = 0;
		}

		delay(1000);
	}
}

