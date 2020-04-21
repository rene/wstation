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
 * \file: EInterface.h
 * \see: EInterface.cpp
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

class EInterface {
	private:
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


		/* Print a temperature value with degree symbol */
		void drawTemp(float temp, int x, int y);

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

		/* Show weather icon */
		void showWeather(weather_t weather, char period);

		/* Show city name */
		void showCity();

		/* Show IP address */
		void showIP();

		/* Draw Indoor/Outdoor labels */
		void showTempLabels();

		/* Show temperature 1 */
		void showTemp1(float temp);

		/* Show temperature 2 */
		void showTemp2(float temp);

		/* Show/hide antenna icon */
		void showRadio(bool show);

		/* Show/hide WiFi icon */
		void showWiFi(bool show);

		/* Show/hide battery 1 icon */
		void showBattery1(bool show);

		/* Show/hide battery 2 icon */
		void showBattery2(bool show);

		/* Draw all graphical elements in the screen */
		void showAll();
};
#endif /* __EINTERFACE_H__ */
