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
 * \file ETheme.h
 * \brief Provide color theme for the LCD screen GUI
 */
#ifndef __ETHEME_H__
#define __ETHEME_H__

#include <string.h>
#include <stdint.h>

/** Color type (16 bits: RGB565) */
typedef uint16_t color_t;

/**
 * \class ETheme
 * \brief Provide color theme for the LCD screen GUI
 * In this class we just define all the colors used in the LCD screen
 */
class ETheme {
	private:
		color_t background;
		color_t ip;
		color_t city;
		color_t date;
		color_t clock;
		color_t tempLabel1;
		color_t temp;
		color_t humidity[3];
		color_t weekday;
		color_t weektemp1;
		color_t weektemp2;
		String icons[22];

	public:
		/** Types of pixmaps in the LCD screen */
		typedef enum _pixmap {
			/** Clear sky: day */
			FIG_01D = 0,
			/** Clear sky: night */
			FIG_01N,
			/** Few clouds: day */
			FIG_02D,
			/** Few clouds: night */
			FIG_02N,
			/** Scattered clouds: day */
			FIG_03D,
			/** Scattered clouds: night */
			FIG_03N,
			/** Broken clouds: day */
			FIG_04D,
			/** Broken clouds: night */
			FIG_04N,
			/** Shower rain: day */
			FIG_09D,
			/** Shower rain: night */
			FIG_09N,
			/** Rain: day */
			FIG_10D,
			/** Rain: night */
			FIG_10N,
			/** Thunderstorm: day */
			FIG_11D,
			/** Thunderstorm: night */
			FIG_11N,
			/** Snow: day */
			FIG_13D,
			/** Snow: night */
			FIG_13N,
			/** Mist: day */
			FIG_50D,
			/** Mist: night */
			FIG_50N,
			/** Radio antenna */
			FIG_RADIO,
			/** WiFi */
			FIG_WIFI,
			/** Battery */
			FIG_BATTERY,
		} pixmap_t;

		/**
		 * \brief Default constructor
		 */
		ETheme() {
			// Initialize colors with default theme
			background  = 0x0000; // RGB(  0,  0,  0)
			ip          = 0x8410; // RGB(128,128,128)
			city        = 0xffff; // RGB(255,255,255)
			date        = 0xffff; // RGB(255,255,255)
			clock       = 0xfa60; // RGB(255, 77,  0)
			tempLabel1  = 0x8410; // RGB(128,128,128)
			temp        = 0xffff; // RGB(255,255,255)
			// Level 0
			humidity[0] = 0xf800; // RGB(255,  0,  0)
			// Level 1
			humidity[1] = 0xf780; // RGB(240,240,  0)
			// Level 2
			humidity[2] = 0x0640; // RGB(0,  200,  0)
			weekday     = 0x8410; // RGB(128,128,128)
			weektemp1   = 0x7d1f; // RGB(120,160,255)
			weektemp2   = 0xf186; // RGB(240, 40, 40)
			// Pixmap files
			icons[FIG_01D] = "/01d.px";
			icons[FIG_01N] = "/01n.px";
			icons[FIG_02D] = "/02d.px";
			icons[FIG_02N] = "/02n.px";
			icons[FIG_03D] = "/03d.px";
			icons[FIG_03N] = "/03n.px";
			icons[FIG_04D] = "/04d.px";
			icons[FIG_04N] = "/04n.px";
			icons[FIG_09D] = "/09d.px";
			icons[FIG_09N] = "/09n.px";
			icons[FIG_10D] = "/10d.px";
			icons[FIG_10N] = "/10n.px";
			icons[FIG_11D] = "/11d.px";
			icons[FIG_11N] = "/11n.px";
			icons[FIG_13D] = "/13d.px";
			icons[FIG_13N] = "/13n.px";
			icons[FIG_50D] = "/50d.px";
			icons[FIG_50N] = "/50n.px";
			icons[FIG_RADIO]   = "/radio.px";
			icons[FIG_WIFI]    = "/wifi.px";
			icons[FIG_BATTERY] = "/battery.px";
		}

		/**
		 * \brief Constructor
		 */
		ETheme(color_t ip, color_t city, color_t date) {
			this->ip   = ip;
			this->city = city;
			this->date = date;
			// TODO
		}

		/**
		 * \brief Return background color
		 * \return color_t Color
		 */
		color_t getBackground() {
			return this->background;
		}

		/**
		 * \brief Return color of IP label
		 * \return color_t Color
		 */
		color_t getIP() {
			return this->ip;
		}
		/**
		 * \brief Return color of City label
		 * \return color_t Color
		 */
		color_t getCity() {
			return this->city;
		}
		/**
		 * \brief Return color of Date label
		 * \return color_t Color
		 */
		color_t getDate() {
			return this->date;
		}
		/**
		 * \brief Return color of Clock label
		 * \return color_t Color
		 */
		color_t getClock() {
			return this->clock;
		}
		/**
		 * \brief Return color for Temperature labels
		 * \return color_t Color
		 */
		color_t getTempLabel() {
			return this->tempLabel1;
		}
		/**
		 * \brief Return color for Temperature values
		 * \return color_t Color
		 */
		color_t getTemperature() {
			return this->temp;
		}
		/**
		 * \brief Return color for Humidity values
		 * \param [in] level Humidity level(0, 1 or 2)
		 * \return color_t Color
		 */
		color_t getHumidity(int level) {
			return this->humidity[level];
		}
		/**
		 * \brief Return the label color for Days of the week in forecast
		 * \return color_t Color
		 */
		color_t getWeekDay() {
			return this->weekday;
		}
		/**
		 * \brief Return the label color for Temperature 1 in forecast 
		 * \return color_t Color
		 */
		color_t getWeekTemp1() {
			return this->weektemp1;
		}
		/**
		 * \brief Return the label color for Temperature 2 in forecast
		 * \return color_t Color
		 */
		color_t getWeekTemp2() {
			return this->weektemp2;
		}
		/**
		 * \brief Return the file name for a given icon
		 * \param [in] pixmap_t Pixmap
		 * \return char* File name
		 */
		String getPixmapFile(pixmap_t pixmap) {
			return this->icons[pixmap];
		}
};
#endif /* __ETHEME_H__ */
