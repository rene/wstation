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
 * \file webservices.cpp
 * \brief Implement web services
 */
#include <WiFi.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <TimeLib.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wstation.h"
#include "webservices.h"

/** Wall clock */
extern tmElements_t wallClock;

/**
 * \brief Format integers into Strings with at least two digits
 * \param [in] int Integer
 * \return String
 */
String format2Dig(int i)
{
	String n = String(i, DEC);
	if (i < 10) {
		return String("0" + n);
	} else {
		return n;
	}
}

/**
 * \brief Process variables from web pages
 * \param [in] var Variable
 * \return String
 */
String processData(const String& var)
{
	if (var == "FIRMWARE_VERSION")
		return String(WSTATION_VERSION);
	else if (var == "WIFI_SSID")
		return confData.getWiFiSSID();
	else if (var == "WIFI_PASS")
		return confData.getWiFiPassword();
	else if (var == "API_KEY")
		return confData.getAPIKey();
	else if (var == "CITY")
		return confData.getCity();
	else if (var == "YEAR")
		return String(tmYearToCalendar(wallClock.Year));
	else if (var == "MONTH")
		return format2Dig(wallClock.Month);
	else if (var == "DAY")
		return format2Dig(wallClock.Day);
	else if (var == "HOURS")
		return format2Dig(wallClock.Hour);
	else if (var == "MINUTES")
		return format2Dig(wallClock.Minute);
	else if (var == "SECONDS")
		return format2Dig(wallClock.Second);
	else if (var == "NTP_SERVER")
		return confData.getNTPServer();
	else if (var == "LCD_BRIGHTNESS")
		return String(confData.getLCDBrightness());

	return String();
}

/**
 * \brief Setup all web services
 * \param [in] webserver AsyncWebServer object
 */
void SetupWebServices(AsyncWebServer *webServer)
{
	if (!webServer)
		return;

	// Main page (configuration)
	webServer->serveStatic("/conf.html", SPIFFS, "/conf.html").setTemplateProcessor(processData);
	//webServer->serveStatic("/", SPIFFS, "/conf.html");

	// Logo image file
	webServer->serveStatic("/logo.png", SPIFFS, "/logo.png");

	// CSS file
	webServer->serveStatic("/wstation.css", SPIFFS, "/wstation.css");
	
	// Java script functions
	webServer->serveStatic("/conf.js", SPIFFS, "/conf.js");

	// Reset function
	webServer->on("/resetDevice", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "application/json", "{\"status\":\"OK\"}");
		// Let's give some time for response
		delay(2000);
		// Acquire mutex to reset device, we should never return from here
		xSemaphoreTake(reset_mutex, portMAX_DELAY);
		esp_restart();
		// Should never reach here: do not release the mutex for safety reasons
	});

	// WiFi scan function
	webServer->on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
		String json = "[";
		int n = WiFi.scanComplete();
		if(n == -2){
			WiFi.scanNetworks(true);
		} else if(n){
			for (int i = 0; i < n; ++i){
				if(i) json += ",";
				json += "{";
				json += "\"ssid\":\""   + WiFi.SSID(i) + "\"";
				json += ",\"rssi\":"    + String(WiFi.RSSI(i));
				json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
				json += ",\"channel\":" + String(WiFi.channel(i));
				json += ",\"secure\":"  + String(WiFi.encryptionType(i));
				json += "}";
			}
			WiFi.scanDelete();
			if(WiFi.scanComplete() == -2){
				WiFi.scanNetworks(true);
			}
		}
		json += "]";
		request->send(200, "application/json", json);
		json = String();
	});
}

