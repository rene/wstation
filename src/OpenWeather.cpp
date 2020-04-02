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
 * @file OpenWeather.cpp
 * @class OpenWeather
 * Provide weather information through OpenWeather API
 */

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "OpenWeather.h"

#define MAX_URL_SIZE  512

/**
 * Constructor
 */
OpenWeather::OpenWeather() :
	key(""), city("")
{
	int i;
	for (i = 0; i < MAX_FORECAST_DAYS; i++) {
		weeklyFC[i].temp     = -999;
		weeklyFC[i].min      = -999;
		weeklyFC[i].max      = -999;
		weeklyFC[i].feels    = -999;
		weeklyFC[i].humidity = -1;
		weeklyFC[i].pressure = -999;
		weeklyFC[i].weather  = CLEAR_SKY;
		weeklyFC[i].date     = 0;
	}
}

/**
 * Constructor
 * @param [in] key API key
 */
OpenWeather::OpenWeather(const String& key)
{
	OpenWeather();
	this->key = key;
}

/**
 * Set city name
 * @param [in] city City name
 */
void OpenWeather::setCity(const String& city)
{
	this->city = city;
}

/**
 * Set API key
 * @param [in] key API key
 */
void OpenWeather::setAPIKey(const String& key)
{
	this->key = key;
}

/**
 * Return city name
 * @return String
 */
const String OpenWeather::getCity()
{
	return this->city;
}

/**
 * Return API key
 * @return String
 */
const String OpenWeather::getAPIKey()
{
	return this->key;
}

/**
 * Retrieve daily forecast from the server
 * @return 0 on success, error number otherwise
 */
int OpenWeather::updateDailyForecast()
{
	char url[MAX_URL_SIZE];
	HTTPClient http;
	int res;

	// URL
	snprintf(url, MAX_URL_SIZE,
			"%s?q=%s&appid=%s", FC_URL_DAILY,
			city.c_str(), key.c_str());

	// Retrieve from server
	http.begin(url);
	res = http.GET();

	// Check result
	if(res > 0) {
		if(res == HTTP_CODE_OK) {
			// Parse information
			res = parseDaily(http.getString());
		} else {
			log_e("HTTP/GET response error: %d", res);
		}
	} else {
		log_e("HTTP/GET error: %d", res);
	}

	http.end();
	return res;
}

/**
 * Retrieve weekly forecast from the server
 * @return 0 on success, error number otherwise
 */
int OpenWeather::updateWeeklyForecast()
{
	char url[MAX_URL_SIZE];
	HTTPClient http;
	int res;

	// URL
	snprintf(url, MAX_URL_SIZE,
			"%s?q=%s&appid=%s&cnt=24", FC_URL_WEEKLY,
			city.c_str(), key.c_str());

	// Retrieve from server
	http.begin(url);
	res = http.GET();

	// Check result
	if(res > 0) {
		if(res == HTTP_CODE_OK) {
			// Parse information
			res = parseWeekly(http.getString());
		} else {
			log_e("HTTP/GET response error: %d", res);
		}
	} else {
		log_e("HTTP/GET error: %d", res);
	}

	http.end();
	return res;
}

/**
 * Retrieve forecast from the server
 * @return 0 on success, error number otherwise
 */
int OpenWeather::updateForecast()
{
	int res = 0;
	res |= updateDailyForecast();
	res |= updateWeeklyForecast();
	return res;
}

/**
 * Get daily forecast
 * @return weather_info_t Weather information
 */
weather_info_t OpenWeather::getDailyForecast()
{
	return dailyFC;
}

/**
 * Get weekly forecast
 * @param [in] i Day index (from 0 to MAX_FORECAST_DAYS)
 * @return weather_info_t Weather information
 */
weather_info_t OpenWeather::getWeeklyForecast(int i)
{
	if (i < MAX_FORECAST_DAYS) {
		return weeklyFC[i];
	} else {
		return weeklyFC[0];
	}
}

/**
 * Convert Kelvin temperature
 * @param [in] k Temperature (in Kelvin)
 * @param [in] scale Scale (CELSIUS or FAHRENHEIT)
 * @return float
 */
float OpenWeather::convKelvinTemp(float k, temp_scale_t scale)
{
	if (scale == FAHRENHEIT) {
		return (k * 1.80) - 459.67;
	} else {
		return k - 273.15;
	}
}

/**
 * Return weather_t type from ID
 * @param [in] id ID
 * @return weather_t
 */
weather_t OpenWeather::getWeatherFromID(int id)
{
	return (weather_t)id;
}


/* ======================= PRIVATE ======================= */

/**
 * Parse daily forecast information
 * @param [in] json JSON string
 * @return int 0 on success, negative number otherwise
 */
int OpenWeather::parseDaily(const String& json)
{
	int id;
	StaticJsonDocument<1024> doc;
	DeserializationError error = deserializeJson(doc, json);

	if (error) {
		log_e("deserializeJson() failed: %s", error.c_str());
		return -1;
	}

	id = doc["weather"][0]["id"];

	dailyFC.temp     = doc["main"]["temp"];
	dailyFC.min      = doc["main"]["temp_min"];
	dailyFC.max      = doc["main"]["temp_max"];
	dailyFC.feels    = doc["main"]["feels_like"];
	dailyFC.pressure = doc["main"]["pressure"];
	dailyFC.humidity = doc["main"]["humidity"];

	dailyFC.weather  = getWeatherFromID(id);
	dailyFC.date     = doc["dt"];

	return 0;
}

/**
 * Parse weekly forecast information
 * @param [in] json JSON string
 * @return int 0 on success, negative number otherwise
 */
int OpenWeather::parseWeekly(const String& json)
{
	int i, j, cnt, lday, cday, id;
	time_t t;
	StaticJsonDocument<20480> doc;
	DeserializationError error = deserializeJson(doc, json);

	if (error) {
		log_e("deserializeJson() failed: %s", error.c_str());
		return -1;
	}

	cnt  = doc["cnt"];
	j    = 0;
	lday = -1;
	for (i = 0; i < cnt; i++) {
		t = doc["list"][i]["dt"];
		cday = day(t);

		if (cday != lday) {
			lday = cday;

			id = doc["list"][i]["weather"][0]["id"];
			weeklyFC[j].temp     = doc["list"][i]["main"]["temp"];
			weeklyFC[j].min      = doc["list"][i]["main"]["temp_min"];
			weeklyFC[j].max      = doc["list"][i]["main"]["temp_max"];
			weeklyFC[j].feels    = doc["list"][i]["main"]["feels_like"];
			weeklyFC[j].pressure = doc["list"][i]["main"]["pressure"];
			weeklyFC[j].humidity = doc["list"][i]["main"]["humidity"];

			weeklyFC[j].weather  = getWeatherFromID(id);
			weeklyFC[j].date     = t;
			j++;

			if (j >= MAX_FORECAST_DAYS)
				break;
		}
	}

	return 0;
}

