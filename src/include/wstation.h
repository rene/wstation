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
 * @file wstation.h
 * Custom types, defines and general configuration
 */
#ifndef __WSTATION_H__
#define __WSTATION_H__

#include <TimeLib.h>

/** Firmware version */
#ifndef WSTATION_VERSION
#define WSTATION_VERSION "0.1beta"
#endif

/** Web server port */
#define WEBSERVER_PORT 80

/** Default WiFi SSID for AP mode */
#define DEFAULT_AP_SSID "WStation"
/** Default WiFi password for AP mode */
#define DEFAULT_AP_PASS "wstation1234"

/** LED pin */
#define LED_PIN 2

/** 433MHz module receiver Data pin */
#define RF_PIN  4

/** DHT sensor data pin */
#define DHT_DATA_PIN 32

/** TFT module DC pin */
#define TFT_DC 16
/** TFT module CS pin */
#define TFT_CS 17
/** TFT module led pin */
#define TFT_BACKLIGHT 19

/** Sensor data display interval (in seconds) */
#define SENSOR_DISPLAY_INTERVAL 5

/** Sensor data expiration period (in seconds) */
#define SENSOR_DATA_EXPIRATION 600

/** Weather information update interval (in seconds) */
#define WEATHER_UPDATE_INTERVAL 60

/** NTP date/time update interval (in seconds) */
#define NTP_UPDATE_INTERVAL 1800

/** Humidity level: Low (dry) */
#define HUMIDITY_L0_LOW    0
/** Humidity level: Comfortable */
#define HUMIDITY_L1_IDEAL 40
/** Humidity level: High */
#define HUMIDITY_L2_HIGH  70

/** Types of pixmaps in the LCD screen */
typedef enum _weather_id {
	/** Unknown */
	UNKNOWN_WEATHER = 0,
	/** Thunderstorm: Light rain */
	THUNDERSTORM_LIGHT_RAIN = 200,
	/** Thunderstorm: Rain */
	THUNDERSTORM_RAIN = 201,
	/** Thunderstorm: Heavy rain */
	THUNDERSTORM_HEAVY_RAIN = 202,
	/** Thunderstorm: Light */
	THUNDERSTORM_LIGHT = 210,
	/** Thunderstorm */
	THUNDERSTORM = 211,
	/** Thunderstorm: Heavy */
	THUNDERSTORM_HEAVY = 212,
	/** Thunderstorm: Ragged */
	THUNDERSTORM_RAGGED = 221,
	/** Thunderstorm: Light drizzle */
	THUNDERSTORM_LIGHT_DRIZZLE = 230,
	/** Thunderstorm: drizzle */
	THUNDERSTORM_DRIZZLE = 231,
	/** Thunderstorm: Heavy drizzle */
	THUNDERSTORM_HEAVY_DRIZZLE = 232,
	/** Drizzle: Light */
	DRIZZLE_LIGHT = 300,
	/** Drizzle */
	DRIZZLE = 301,
	/** Drizzle: Heavy */
	DRIZZLE_HEAVY = 302,
	/** Drizzle: Light drizzle rain */
	DRIZZLE_LIGHT_RAIN = 310,
	/** Drizzle: Rain */
	DRIZZLE_RAIN = 311,
	/** Drizzle: Heavy drizzle rain */
	DRIZZLE_HEAVY_RAIN = 312,
	/** Drizzle: Shower rain */
	DRIZZLE_SHOWER_RAIN = 313,
	/** Drizzle: Heavy shower rain and drizzle */
	DRIZZLE_HEAVY_SHOWER_RAIN = 314,
	/** Drizzle: Shower */
	DRIZZLE_SHOWER = 321,
	/** Rain: Light */
	RAIN_LIGHT = 500,
	/** Rain: Moderate */
	RAIN_MODERATE = 501,
	/** Rain: Heavy */
	RAIN_HEAVY = 502,
	/** Rain: Very heavy */
	RAIN_VERY_HEAVY = 503,
	/** Rain: Extreme */
	RAIN_EXTREME = 504,
	/** Rain: Freezing */
	RAIN_FREEZING = 511,
	/** Rain: Light shower */
	RAIN_LIGHT_SHOWER = 520,
	/** Rain: Shower */
	RAIN_SHOWER = 521,
	/** Rain: Heavy shower */
	RAIN_HEAVY_SHOWER = 522,
	/** Rain: Ragged */
	RAIN_RAGGED = 531,
	/** Snow: Light */
	SNOW_LIGHT = 600,
	/** Snow */
	SNOW = 601,
	/** Snow: Heavy */
	SNOW_HEAVY = 602,
	/** Snow: Sleet */
	SNOW_SLEET = 611,
	/** Snow: Light shower sleet */
	SNOW_LSHOWER_SLEET = 612,
	/** Snow: Shower sleet */
	SNOW_SHOWER_SLEET = 613,
	/** Snow: XX */
	SNOW_LIGHT_RAIN = 615,
	/** Snow: XX */
	SNOW_RAIN = 616,
	/** Snow: Light shower */
	SNOW_LIGHT_SHOWER = 620,
	/** Snow: Shower */
	SNOW_SHOWER = 621,
	/** Snow: Heavy shower */
	SNOW_HEAVY_SHOWER = 622,
	/** Atmosphere: Mist */
	ATM_MIST = 701,
	/** Atmosphere: Smoke */
	ATM_SMOKE = 711,
	/** Atmosphere: Haze */
	ATM_HAZE = 721,
	/** Atmosphere: Dust whirls */
	ATM_DUST_WHIRLS = 731,
	/** Atmosphere: Fog */
	ATM_FOG = 741,
	/** Atmosphere: Sand */
	ATM_SAND = 751,
	/** Atmosphere: Dust */
	ATM_DUST = 761,
	/** Atmosphere: Volcanic ash */
	ATM_VOLCANIC_ASH = 762,
	/** Atmosphere: Squalls */
	ATM_SQUALL = 771,
	/** Atmosphere: XX */
	ATM_TORNADO = 781,
	/** Clear */
	CLEAR_SKY = 800,
	/** Clouds: Few */
	CLOUDS_FEW = 801,
	/** Clouds: Scattered */
	CLOUDS_SCATTERED = 802,
	/** Clouds: Broken */
	CLOUDS_BROKEN = 803,
	/** Clouds: Overcast */
	CLOUDS_OVERCAST = 804,
} weather_t;

/** Temperature scale */
typedef enum _temp_scale {
	/** Celsius */
	CELSIUS = 0,
	/** Fahrenheit */
	FAHRENHEIT,
} temp_scale_t;

/** Wall clock */
extern tmElements_t wallClock;

/* Prototypes */

void userSetupDone(void);
void updateFromConf(void);

#endif /* __WSTATION_H__ */
