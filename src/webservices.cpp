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
 * @file webservices.cpp
 * Implement web services
 */
#include <WiFi.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include "wstation.h"
#include "webservices.h"
#include "EInterface.h"

#define CHECK_HTTP_AUTH(req, conf) do { \
	if(!req->authenticate(conf.getUsername().c_str(), \
							conf.getUserPass().c_str())) \
      return req->requestAuthentication(); \
	} while(0)

static String checkGetParam(AsyncWebServerRequest *request, String param)
{
	String field("");
	if (request->hasParam(param, true)) {
		field = request->getParam(param, true)->value();
	}
	return field;
}

/**
 * Calculate the day of the week
 * Algorithm from: https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
 * @param [in] y Year
 * @param [in] m Month
 * @param [in] d Day
 * @return int Day of the week
 */
static int dayofweek(int y, int m, int d)	/* 1 <= m <= 12,  y > 1752 (in the U.K.) */
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	if( m < 3 )
	{
		y -= 1;
	}
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

/**
 * Format integers into Strings with at least two digits
 * @param [in] int Integer
 * @return String
 */
static String format2Dig(int i)
{
	String n = String(i, DEC);
	if (i < 10) {
		return String("0" + n);
	} else {
		return n;
	}
}

/**
 * Process variables from web pages
 * @param [in] var Variable
 * @return String
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
	else if (var == "TIMEZONE")
		return String(confData.getTimezone());
	else if (var == "DAYLIGHT")
		return String(confData.getDaylight());
	else if (var == "USERNAME")
		return String(confData.getUsername());
	else if (var == "USERPASS")
		return String(confData.getUserPass());
	else if (var == "TEMPSCALE")
		if (confData.getTempScale() == FAHRENHEIT) {
			return String("F");
		} else {
			return String("C");
		}

	return String();
}

/**
 * Setup all web services
 * @param [in] webserver AsyncWebServer object
 */
void SetupWebServices(AsyncWebServer *webServer)
{
	if (!webServer)
		return;

	// Main page (configuration)
	webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		CHECK_HTTP_AUTH(request, confData);
		request->send(SPIFFS, "/conf.html", "text/html", false, processData);
	});

	// Logo image file
	webServer->serveStatic("/logo.png", SPIFFS, "/logo.png");

	// CSS file
	webServer->serveStatic("/wstation.css", SPIFFS, "/wstation.css");
	
	// Java script functions
	webServer->serveStatic("/conf.js", SPIFFS, "/conf.js");

	// Login page
	webServer->serveStatic("/login", SPIFFS, "/login.html");

	// Logout function
	webServer->on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(401);
	});

	// Firmware uploading and update
	webServer->on("/updateFirmware", HTTP_POST,
		[](AsyncWebServerRequest *request) {
			CHECK_HTTP_AUTH(request, confData);
			request->send(200);
		},
		[](AsyncWebServerRequest *request,
				const String& filename, size_t index, uint8_t *data,
				size_t len, bool final) {
			CHECK_HTTP_AUTH(request, confData);
			if(index == 0) {
				// Start firmware update
				log_i("FIRMWARE UPDATE REQUESTED: %s", filename.c_str());
				if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
					log_e("FIRMWARE UPDATE FAILED: %s", Update.errorString());
				}
			}

			if (len > 0) {
				// Write to flash
				if (Update.write(data, len) != len) {
					log_e("FIRMWARE UPDATE: WRITING DATA ERROR!");
				}
			}

			if(final) {
				// Update is done
				size_t total_size = index + len;
				log_i("FIRMWARE UPLOAD DONE: %s (%u)", filename.c_str(), total_size);
				if (Update.end(true)) {
					request->send(200);
					log_i("FIRMWARE UPDATE SUCCESS: %u", total_size);
					delay(1000);
					esp_restart();
				} else {
					log_e("FIRMWARE UPDATE FAILED: %s", Update.errorString());
					request->send(408, "text/plain", "Firmware Update Error.");
				}
			}
		});

	// Reset function
	webServer->on("/resetDevice", HTTP_GET, [](AsyncWebServerRequest *request){
		CHECK_HTTP_AUTH(request, confData);
		request->send(200, "application/json", "{\"status\":\"OK\"}");
		// Let's give some time for response
		delay(2000);
		// Acquire mutex to reset device, we should never return from here
		xSemaphoreTake(reset_mutex, portMAX_DELAY);
		esp_restart();
		// Should never reach here: do not release the mutex for safety reasons
	});

	// Reset to factory settings
	webServer->on("/resetToFactory", HTTP_GET, [](AsyncWebServerRequest *request){
		CHECK_HTTP_AUTH(request, confData);
		request->send(200, "application/json", "{\"status\":\"OK\"}");
		// Let's give some time for response
		delay(2000);
		// Perform factory reset
		factoryReset();
	});

	// Save configuration data
	webServer->on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
		CHECK_HTTP_AUTH(request, confData);
		confData.setWiFiSSID(checkGetParam(request, PARAM_SSID));
		confData.setWiFiPassword(checkGetParam(request, PARAM_WIFIPASS));
		confData.setAPIKey(checkGetParam(request, PARAM_KEY));
		confData.setCity(checkGetParam(request, PARAM_CITY));

		confData.setNTPServer(checkGetParam(request, PARAM_NTP));

		int tz = checkGetParam(request, PARAM_TIMEZONE).toInt();
		confData.setTimezone(tz);

		if (checkGetParam(request, PARAM_DAYLIGHT) == "on") {
			confData.setDaylight(3600);
		} else {
			confData.setDaylight(0);
		}

		String dt = checkGetParam(request, PARAM_DATE);
		if (dt.length() >= 10) {
			int year  = dt.substring(0, 4).toInt();
			int month = dt.substring(5, 7).toInt();
			int day   = dt.substring(8, 10).toInt();
			int wday  = dayofweek(year, month, day);
			confData.setDate(day, month, year, wday);
		}

		int h = checkGetParam(request, PARAM_HOURS).toInt();
		int m = checkGetParam(request, PARAM_MINUTES).toInt();
		int s = checkGetParam(request, PARAM_SECONDS).toInt();
		confData.setHours(h);
		confData.setMinutes(m);
		confData.setSeconds(s);

		int lcdbrig = checkGetParam(request, PARAM_LCDBRIG).toInt();
		if (lcdbrig > 0 && lcdbrig <= 255) {
			confData.setLCDBrightness(lcdbrig);
		}

		String tc = checkGetParam(request, PARAM_TEMPSCALE);
		if (tc == "F") {
			confData.setTempScale(FAHRENHEIT);
		} else {
			confData.setTempScale(CELSIUS);
		}

		confData.setUsername(checkGetParam(request, PARAM_USERNAME));
		confData.setUserPass(checkGetParam(request, PARAM_USER_PASS));

		if (!confData.isConfigured()) {
			// This is the first setup, we need to reset the device
			confData.SaveConf();
			userSetupDone();
		} else {
			confData.SaveConf();
			updateFromConf();
		}

		request->redirect("/");
    });

	// WiFi scan function
	webServer->on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
		CHECK_HTTP_AUTH(request, confData);
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

