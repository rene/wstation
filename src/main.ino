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
#include <DS1307RTC.h>
#include "wstation.h"
#include "nexus.h"
#include "ETheme.h"
#include "EInterface.h"
#include "OpenWeather.h"

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
 * \brief Update forecast information on the screen
 */
void taskUpdateWeatherInfo(void *parameter)
{
	int i;
	float t1, tf1, tf2;
	weather_info_t wfc;

	while (1) {
		if (wifiMulti.run() == WL_CONNECTED) {
			if ((now() - lastUpdate) >= 60) {
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
 * \brief Receive nexus sensor data
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
	delay(700);

	// Initialize 433MHz module receiver
	setupNexus(RF_PIN);


	// DEMO
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

	xTaskCreate(taskUpdateScreen,      "UpdateScreen",      10240, NULL, 2, NULL);
	xTaskCreate(taskReceiveSensorData, "ReceiveSensorData", 10240, NULL, 0, NULL);
	xTaskCreate(taskUpdateWeatherInfo, "UpdateWeatherInfo", 32768, NULL, 0, NULL);
#endif
}

/**
 * \brief Main loop
 */
void loop()
{
	if (wifiMulti.run() == WL_CONNECTED) {
		xSemaphoreTake(t_mutex, portMAX_DELAY);
		gui->showWiFi(true);
		char ip[40]; IPAddress ipAddr = WiFi.localIP();
		snprintf(ip, 40, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
		gui->setIP(ip);
		xSemaphoreGive(t_mutex);
	} else {
		xSemaphoreTake(t_mutex, portMAX_DELAY);
		gui->showWiFi(false);
		gui->setIP("");
		xSemaphoreGive(t_mutex);
	}

	delay(5000);
}

