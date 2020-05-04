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
 * \file: OpenWeather.h
 * \see: OpenWeather.cpp
 */
#ifndef __OPENWEATHER_H__
#define __OPENWEATHER_H__

#include <Time.h>
#include <wstation.h>

/** URL for daily forecast */
#define FC_URL_DAILY "http://api.openweathermap.org/data/2.5/weather"
/** URL for weekly forecast */
#define FC_URL_WEEKLY "http://api.openweathermap.org/data/2.5/forecast"
/** Maximum days for forecast */
#define MAX_FORECAST_DAYS 7

/** Forecast information structure */
typedef struct _weather_info {
	/** Current temperature */
	float temp;
	/** Minimum temperature */
	float min;
	/** Maximum temperature */
	float max;
	/** Feels like temperature */
	float feels;
	/** Humidity */
	int humidity;
	/** Pressure */
	float pressure;
	/** Current weather */
	weather_t weather;
	/** Date for the forecast */
	time_t date;
} weather_info_t;


class OpenWeather {
	private:
		/** API key */
		String key;
		/** City */
		String city;
		/** Daily forecast */
		weather_info_t dailyFC;
		/** Weekly forecast */
		weather_info_t weeklyFC[MAX_FORECAST_DAYS];

		/* Parse daily forecast information */
		int parseDaily(const String& json);
		/* Parse weekly forecast information */
		int parseWeekly(const String& json);

	public:
		/* Constructor */
		OpenWeather();

		/* Constructor */
		OpenWeather(const String& key);

		/* Set city name */
		void setCity(const String& city);

		/* Set API key */
		void setAPIKey(const String& city);

		/* Retrieve daily forecast from the server */
		int updateDailyForecast();

		/* Retrieve weekly forecast from the server */
		int updateWeeklyForecast();

		/* Retrieve forecast from the server */
		int updateForecast();

		/* Get daily forecast */
		weather_info_t getDailyForecast();

		/* Convert Kelvin temperature */
		static float convKelvinTemp(float k, temp_scale_t scale);

 		/* Return weather_t type from ID */
		static weather_t getWeatherFromID(int id);
};
#endif /* __OPENWEATHER_H__ */
