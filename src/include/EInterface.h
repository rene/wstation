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
 * \file EInterface.h
 * \see EInterface.cpp
 */
#ifndef __EINTERFACE_H__
#define __EINTERFACE_H__

#include <FS.h>
#include <Adafruit_ILI9341.h>
#include <wstation.h>
#include <ETheme.h>

/** Backlight: minimum level */
#define BACKLIGHT_MIN      0x32
/** Backlight: default level */
#define BACKLIGHT_DEFAULT  0xc8
/** Backlight: maximum level */
#define BACKLIGHT_MAX      0xff

/** Invalid temperature */
#define GUI_INV_TEMP     -1E6
/** Invalid humidity */
#define GUI_INV_HUMIDITY -1
/** Invalid channel */
#define GUI_INV_CHANNEL  -1

class EInterface {
	private:
		/** Clock elements */
		typedef enum _clock_el {
			CLOCK_ALL,
			CLOCK_HOURS,
			CLOCK_MINUTES,
			CLOCK_SECONDS
		} clock_el_t;
		/** GUI state */
		bool state;
		/** TFT screen: CS pin */
		int8_t tftCS;
		/** TFT screen: DC pin */
		int8_t tftDC;
		/** TFT screen: Backlight pin */
		int8_t tftLED;
		/** Backlight level */
		int backlight;
		/** TFT module */
		Adafruit_ILI9341 *tft;
		/** Color theme */
		ETheme theme;
		/** File system */
		fs::FS *pfs;
		/** Hours */
		int hours;
		/** Minutes */
		int minutes;
		/** Seconds */
		int seconds;
		/** Temperature 1 */
		float temp1;
		/** Temperature 2 */
		float temp2;
		/** Temperature scale */
		temp_scale_t tempScale;
		/** Weather */
		weather_t weather;
		/** Period: day or night */
		char period;
		/** City */
		String city;
		/** Date */
		String date;
		/** Show radio icon */
		bool radio;
		/** Show WiFi icon */
		bool wifi;
		/** Show radio icon */
		bool battery1;
		/** Show radio icon */
		bool battery2;
		/** IP address */
		String ip;
		/** Humidity 1 */
		int humidity1;
		/** Humidity 2 */
		int humidity2;
		/** Outdoor sensor's channel */
		int channel;
		/** Forecast labels */
		String forecastLabels[3];
		/** Forecast Temperature 1 */
		float forecastTemp1[3];
		/** Forecast Temperature 2 */
		float forecastTemp2[3];
		/** Forecast weather */
		weather_t forecastWeather[3];


		/* Print a temperature value with degree symbol */
		void drawTemp(float temp, int x, int y);

		/* Print humidity value */
		void drawHumidity(int humidity, int x, int y);

		/* Print a forecast temperature */
		void drawForecastTemp(float temp, int x, int y, int16_t color);

		/* Read 16 bits number from file */
		uint16_t readInt(File f);

 		/* Draw a pixel map file on the screen */
		void drawPixmap(int x, int y, String file);

		/* Draw a pixel map file at half size on the screen */
		void drawPixmapHalf(int x, int y, String file);

		/* Convert weather type into the corresponding icon */
		ETheme::pixmap_t getWeatherIcon(weather_t weather, char period);

	public:
		/* Constructor */
		EInterface(int8_t cs, int8_t dc,
				int8_t led, int backlight, ETheme theme,
				fs::FS *pfs);

		/* Destructor */
		~EInterface();

		/* Initialize interface */
		void initialize();

		/* Set backlight level */
		void setBacklight(int level);

		/* Set temperature scale */
		void setTempScale(temp_scale_t scale);

		/* Set city name */
		void setCity(const String& city);

		/* Set IP address */
		void setIP(const String& ip);

		/* Set clock: hours */
		void setHours(int hours);

		/* Set clock: minutes */
		void setMinutes(int minutes);

		/* Set clock: seconds */
		void setSeconds(int seconds);

		/* Show clock */
		void showClock(clock_el_t elements);

		/* Show weather icon */
		void showWeather(weather_t weather, char period);

		/* Show city name */
		void showCity();

		/* Set date */
		void setDate(const String& date);

		/* Show IP address */
		void showIP();

		/* Show date */
		void showDate();

		/* Draw Indoor/Outdoor labels */
		void showTempLabels();

		/* Show temperature 1 */
		void showTemp1(float temp);

		/* Show temperature 2 */
		void showTemp2(float temp);

		/* Show humidity 1 */
		void showHumidity1(int humidity);

		/* Show humidity 2 */
		void showHumidity2(int humidity);

		/* Show sensor's channel */
		void showChannel(int channel);

		/* Show forecast weather icon */
		void showForecastWeather(int i, weather_t weather);

		/* Show forecast label */
		void showForecastLabel(int i, const String& label);

		/* Show forecast temperature 1 */
		void showForecastTemp1(int i, float temp);

		/* Show forecast temperature 2 */
		void showForecastTemp2(int i, float temp);

		/* Show/hide antenna icon */
		void showRadio(bool show);

		/* Show/hide WiFi icon */
		void showWiFi(bool show);

		/* Show/hide battery 1 icon */
		void showBattery1(bool show);

		/* Show/hide battery 2 icon */
		void showBattery2(bool show);

		/* Show WStation logo */
		void showLogo(int x, int y);

		/* Show WStation logo at the center */
		void showLogo();

		/* Show version */
		void showVersion(int x, int y);

		/* Draw all graphical elements in the screen */
		void showAll();

		/* Clear the whole screen */
		void clearAll();

		/* Print text */
		void print(const String& text);

		/* Print text */
		void print(int x, int y, const String& text);

		/* Print text */
		void print(int16_t color, int16_t bgcolor, const String& text);

		/* Print text */
		void print(int x, int y, int16_t color, int16_t bgcolor, const String& text);
};
#endif /* __EINTERFACE_H__ */
