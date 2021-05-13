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
 * @file EInterface.cpp
 * @class EInterface
 * Provide all the methods to control and draw the embedded LCD screen
 */
#include <EInterface.h>
#include <SPI.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#ifdef DEBUG_SCREENSHOT
#include <esp_task_wdt.h>
#endif

/** Default weather */
#define DEF_WEATHER  UNKNOWN_WEATHER
/** Default temperature scale */
#define DEF_SCALE    CELSIUS
/** Default time format */
#define DEF_TIME_FORMAT TIME_FORMAT_24H

/**
 * Constructor
 * @param [in] cs TFT module CS pin
 * @param [in] dc TFT module DC pin
 * @param [in] led TFT module LED pin
 * @param [in] backlight Backlight level
 * @param [in] pfs File system to load pixmap files
 * @param [in] theme Color theme
 */
EInterface::EInterface(int8_t cs, int8_t dc,
		int8_t led, int backlight, ETheme theme,
		fs::FS *pfs) :
	state(false), tftCS(cs), tftDC(dc), tftLED(led),
	backlight(backlight),theme(theme), pfs(pfs),
	tft(NULL), hours(-1), minutes(-1), seconds(-1),
	temp1(GUI_INV_TEMP), temp2(GUI_INV_TEMP), tempScale(DEF_SCALE),
	city(""), date(""), weather(DEF_WEATHER), period(0),
	radio(false), wifi(false), battery1(false), battery2(false),
	ip(""), humidity1(GUI_INV_HUMIDITY), humidity2(GUI_INV_HUMIDITY),
	channel(GUI_INV_CHANNEL), forecastLabels({"---", "---", "---"}),
	forecastTemp1({GUI_INV_TEMP, GUI_INV_TEMP, GUI_INV_TEMP}),
	forecastTemp2({GUI_INV_TEMP, GUI_INV_TEMP, GUI_INV_TEMP}),
	forecastWeather({DEF_WEATHER, DEF_WEATHER, DEF_WEATHER}),
	timeFormat(DEF_TIME_FORMAT)
{
	this->tft = new Adafruit_ILI9341(tftCS, tftDC);
}

/**
 * Destructor
 */
EInterface::~EInterface()
{
	if (this->tft)
		delete this->tft;
}

/**
 * Initialize interface
 */
void EInterface::initialize()
{
	// Initialize TFT module
	tft->begin();
	tft->setRotation(2);
	tft->fillScreen(theme.getBackground());

	// Initialize backlight PWM
	// channel: 0, freq: 5KHz, resolution: 8 bits
	ledcSetup(0, 5000, 8);
	ledcAttachPin(tftLED, 0);
	ledcWrite(0, backlight);

	state = true;
}

/**
 * Set backlight level
 * @param [in] level Level
 */
void EInterface::setBacklight(int level)
{
	this->backlight = level;
	ledcWrite(0, level);
}

/**
 * Set temperature scale
 * @param [in] scale Scale
 * \note This parameter only changes the degree symbol. No conversion is made.
 */
void EInterface::setTempScale(temp_scale_t scale)
{
	int i;
	tempScale = scale;
	showTemp1(temp1);
	showTemp2(temp2);
	for (i = 0; i < 3; i++) {
		showForecastWeather(i, forecastWeather[i]);
		showForecastLabel(i, forecastLabels[i]);
		showForecastTemp1(i, forecastTemp1[i]);
		showForecastTemp2(i, forecastTemp2[i]);
	}
}

/**
 * Set time format
 * @param [in] timeFormat Time format
 */
void EInterface::setTimeFormat(time_format_t timeFormat)
{
	this->timeFormat = timeFormat;
	showClock(CLOCK_HOURS);
}

/**
 * Set city name
 * @param [in] city City name
 */
void EInterface::setCity(const String& city)
{
	this->city = city;
	showCity();
}

/**
 * Set date
 * @param [in] date Date string representation
 */
void EInterface::setDate(const String& date)
{
	this->date = date;
	showDate();
}

/**
 * Set clock: hours
 * @param [in] hours Hours
 */
void EInterface::setHours(int hours)
{
	if (this->hours != hours) {
		this->hours = hours;
		showClock(CLOCK_HOURS);
	}
}

/**
 * Set clock: minutes
 * @param [in] minutes Minutes
 */
void EInterface::setMinutes(int minutes)
{
	if (this->minutes != minutes) {
		this->minutes = minutes;
		showClock(CLOCK_MINUTES);
	}
}

/**
 * Set clock: seconds
 * @param [in] seconds Seconds
 */
void EInterface::setSeconds(int seconds)
{
	if (this->seconds != seconds) {
		this->seconds = seconds;
		showClock(CLOCK_SECONDS);
	}
}

/**
 * Show clock
 * @param [in] elements Clock elements to show
 */
void EInterface::showClock(clock_el_t elements)
{
	int16_t x1, y1;
	uint16_t w, h;
	int hrs;
	char str[4];

	// Hours
	if (elements == CLOCK_ALL || elements == CLOCK_HOURS) {
		if (this->hours >= 0 && this->hours <= 23) {
			tft->setTextColor(theme.getClock());

			if (this->timeFormat == TIME_FORMAT_12H) {
				if (this->hours >= 12) {
					// PM
					if (this->hours == 12) {
						hrs = 12;
					} else {
						hrs = this->hours - 12;
					}
					strcpy(str, "pm");
				} else {
					// AM
					if (this-> hours == 0) {
						hrs = 12;
					} else {
						hrs = this->hours;
					}
					strcpy(str, "am");
				}
			} else {
				hrs = this->hours;
				strcpy(str, "");
			}
			// Print am/pm
			tft->setFont(&FreeSans9pt7b);
			tft->setCursor(190, 95);
			tft->getTextBounds("pm", 190, 95, &x1, &y1, &w, &h);
			tft->fillRect(x1 - 2, y1 - 2, w + 2, h + 2, theme.getBackground());
			tft->print(str);

			// Print hours
			tft->setFont(&FreeSansBold18pt7b);
			tft->setCursor(60, 95);
			snprintf(str, sizeof(str), "%02d:", hrs);

			tft->getTextBounds(str, 60, 95, &x1, &y1, &w, &h);
			tft->fillRect(x1 - 2, y1 - 2, w + 2, h + 2, theme.getBackground());
			tft->print(str);
		}
	}

	// Minutes
	if (elements == CLOCK_ALL || elements == CLOCK_MINUTES) {
		if (this->minutes >= 0 && this->minutes <= 59) {
			tft->setFont(&FreeSansBold18pt7b);
			tft->setTextColor(theme.getClock());
			tft->setCursor(110, 95);
			snprintf(str, sizeof(str), "%02d", this->minutes);

			tft->getTextBounds(str, 110, 95, &x1, &y1, &w, &h);
			tft->fillRect(x1 - 2, y1 - 2, w + 8, h + 2, theme.getBackground());
			tft->print(str);
		}
	}

	// Seconds
	if (elements == CLOCK_ALL || elements == CLOCK_SECONDS) {
		if (this->seconds >= 0 && this->seconds <= 59) {
			tft->setFont(&FreeSansBold12pt7b);
			tft->setTextColor(theme.getClock());
			tft->setCursor(155, 95);
			snprintf(str, sizeof(str), "%02d", this->seconds);

			tft->getTextBounds(str, 155, 95, &x1, &y1, &w, &h);
			tft->fillRect(x1 - 2, y1 - 2, w + 5, h + 2, theme.getBackground());
			tft->print(str);
		}
	}
}

/**
 * Set IP address
 * @param [in] ip IP address
 */
void EInterface::setIP(const String& ip)
{
	this->ip = ip;
	showIP();
}

/**
 * Show weather icon
 * @param [in] weather Weather
 * @param [in] Period: 0 = Day, 1 = Night
 */
void EInterface::showWeather(weather_t weather, char period)
{
	ETheme::pixmap_t icon;

	this->weather = weather;
	this->period  = period;
	icon = getWeatherIcon(weather, period);
	tft->fillRect(0, 18, 60, 60, theme.getBackground());
	drawPixmap(0, 18, theme.getPixmapFile(icon));
}

/**
 * Show city name
 */
void EInterface::showCity()
{
	int16_t x1, y1;
	uint16_t w, h;

	tft->setFont(&FreeSansBold12pt7b);
	tft->setCursor(70,40);
	tft->setTextColor(theme.getCity());
	tft->setTextSize(1);

	tft->getTextBounds(city, 70, 40, &x1, &y1, &w, &h);
	tft->fillRect(x1, y1, w, h + 1, theme.getBackground());
	tft->print(city);
}

/**
 * Show IP address
 */
void EInterface::showIP()
{
	int16_t x1, y1;
	uint16_t w, h;

	tft->setFont(&FreeMono9pt7b);
	tft->setCursor(50, 10);
	tft->setTextColor(theme.getIP());

	// Clear text area (maximum size)
	tft->getTextBounds("000.000.000.000", 50, 10, &x1, &y1, &w, &h);
	tft->fillRect(x1, y1, w, h + 1, theme.getBackground());

	// Right justified
	tft->getTextBounds(ip, 50, 10, &x1, &y1, &w, &h);
	tft->setCursor(210 - w, 10);
	tft->print(ip);
}

/**
 * Show date
 */
void EInterface::showDate()
{
	int16_t x1, y1;
	uint16_t w, h;

	tft->setFont(&FreeSans9pt7b);
	tft->setCursor(70,55);
	tft->setTextColor(theme.getDate());

	tft->getTextBounds("Ap", 70, 55, &x1, &y1, &w, &h);
	tft->fillRect(x1, y1, (320 - x1), h + 1, theme.getBackground());
	tft->print(date);
}

/**
 * Draw Indoor/Outdoor labels
 */
void EInterface::showTempLabels()
{
	tft->setFont(&FreeSans9pt7b);
	tft->setTextColor(theme.getTempLabel());

	tft->setCursor(5, 120);
	tft->print("Indoor:");
	tft->setCursor(5, 185);
	tft->print("Outdoor:");
}

/**
 * Show temperature 1
 * @param [in] temp Temperature (in Celsius)
 */
void EInterface::showTemp1(float temp)
{
	temp1 = temp;
	drawTemp(convCelsius(temp1), 5, 160);
}

/**
 * Show temperature 2
 * @param [in] temp Temperature (in Celsius)
 */
void EInterface::showTemp2(float temp)
{
	temp2 = temp;
	drawTemp(convCelsius(temp2), 5, 225);
}

/**
 * Show humidity 1
 * @param [in] humidity Humidity
 */
void EInterface::showHumidity1(int humidity)
{
	humidity1 = humidity;
	drawHumidity(humidity1, 150, 160);
}

/**
 * Show humidity 2
 * @param [in] humidity Humidity
 */
void EInterface::showHumidity2(int humidity)
{
	humidity2 = humidity;
	drawHumidity(humidity2, 150, 225);
}

/**
 * Show sensor's channel
 * @param [in] channel Channel number
 */
void EInterface::showChannel(int channel)
{
	int16_t x1, y1;
	uint16_t w, h;
	char str[4];

	tft->setFont(&FreeSans9pt7b);
	tft->setTextColor(theme.getTempLabel());
	tft->setCursor(80, 185);

	tft->getTextBounds("000", 80, 185, &x1, &y1, &w, &h);
	tft->fillRect(x1, y1, w, h + 1, theme.getBackground());

	if (channel != GUI_INV_CHANNEL) {
		this->channel = channel;
		snprintf(str, sizeof(str), "%3d", channel);
		tft->print(str);
	} else {
		tft->print("   ");
	}
}

/**
 * Show forecast weather icon
 * @param [in] i Forecast number
 * @param [in] weather Weather
 */
void EInterface::showForecastWeather(int i, weather_t weather)
{
	int x, y;
	ETheme::pixmap_t icon;

	if (i < 0 || i > 3)
		return;

	y = 240;
	switch(i) {
		case 0:
			x = 5;
			break;

		case 1:
			x = 90;
			break;

		case 2:
			x = 168;
			break;
	}

	this->forecastWeather[i] = weather;
	icon = getWeatherIcon(weather, period);

	tft->fillRect(x, y, 30, 30, theme.getBackground());
	drawPixmapHalf(x, y, theme.getPixmapFile(icon));
}

/**
 * Show forecast label
 * @param [in] i Forecast number
 * @param [in] label Label
 */
void EInterface::showForecastLabel(int i, const String& label)
{
	int16_t x1, y1;
	uint16_t w, h;
	int x, y;

	if (i < 0 || i > 3)
		return;

	y = 260;
	switch(i) {
		case 0:
			x = 40;
			break;

		case 1:
			x = 125;
			break;

		case 2:
			x = 203;
			break;
	}

	forecastLabels[i] = label;
	tft->setFont(&FreeSans9pt7b);
	tft->setTextColor(theme.getWeekDay());
	tft->setCursor(x, y);

	tft->getTextBounds("AAA", x, y, &x1, &y1, &w, &h);
	tft->fillRect(x1, y1, w, h + 1, theme.getBackground());
	tft->print(forecastLabels[i]);
}

/**
 * Show forecast temperature 1
 * @param [in] i Forecast number
 * @param [in] temp Temperature (in Celsius)
 */
void EInterface::showForecastTemp1(int i, float temp)
{
	int x, y;

	if (i < 0 || i > 3)
		return;

	y = 305;
	switch(i) {
		case 0:
			x = 5;
			break;

		case 1:
			x = 90;
			break;

		case 2:
			x = 168;
			break;

		default:
			x = 5;
			break;
	}

	forecastTemp1[i] = temp;
	drawForecastTemp(convCelsius(temp), x, y, theme.getWeekTemp1());
}

/**
 * Show forecast temperature 2
 * @param [in] i Forecast number
 * @param [in] temp Temperature (in Celsius)
 */
void EInterface::showForecastTemp2(int i, float temp)
{
	int x, y;

	if (i < 0 || i > 3)
		return;

	y = 285;
	switch(i) {
		case 0:
			x = 5;
			break;

		case 1:
			x = 90;
			break;

		case 2:
			x = 168;
			break;
	}

	forecastTemp2[i] = temp;
	drawForecastTemp(convCelsius(temp), x, y, theme.getWeekTemp2());
}

/**
 * Show/hide antenna icon
 * @param [in] show Show or hide the icon
 */
void EInterface::showRadio(bool show)
{
	this->radio = show;
	if (show) {
		drawPixmap(180, 170, theme.getPixmapFile(ETheme::FIG_RADIO));
	} else  {
		tft->fillRect(180, 170, 14, 24, theme.getBackground());
	}
}

/**
 * Show/hide WiFi icon
 * @param [in] show Show or hide the icon
 */
void EInterface::showWiFi(bool show)
{
	this->wifi = show;
	if (show) {
		drawPixmap(216, 0, theme.getPixmapFile(ETheme::FIG_WIFI));
	} else  {
		tft->fillRect(216, 0, 24, 24, theme.getBackground());
	}
}

/**
 * Show/hide battery 1 icon
 * @param [in] show Show or hide the icon
 */
void EInterface::showBattery1(bool show)
{
	this->battery1 = show;
	if (show) {
		drawPixmap(200, 107, theme.getPixmapFile(ETheme::FIG_BATTERY));
	} else  {
		tft->fillRect(200, 107, 32, 15, theme.getBackground());
	}
}

/**
 * Show/hide battery 2 icon
 * @param [in] show Show or hide the icon
 */
void EInterface::showBattery2(bool show)
{
	this->battery2 = show;
	if (show) {
		drawPixmap(200, 172, theme.getPixmapFile(ETheme::FIG_BATTERY));
	} else  {
		tft->fillRect(200, 172, 32, 15, theme.getBackground());
	}
}

/**
 * Show WStation logo at the center
 */
void EInterface::showLogo()
{
	showLogo(-1, -1);
}

/**
 * Show WStation logo
 * @param [in] x X position (-1 for center position)
 * @param [in] y Y position (-1 for center position)
 */
void EInterface::showLogo(int x, int y)
{
	int x1 = x;
	int y1 = y;

	if (x < 0)
		x1 = 10;
	if (y < 0)
		y1 = 124;

	drawPixmap(x1, y1, theme.getPixmapFile(ETheme::FIG_LOGO));
}

/**
 * Show firmware version
 * @param [in] x X position (-1 for center position)
 * @param [in] y Y position (-1 for center position)
 */
void EInterface::showVersion(int x, int y)
{
	int16_t x1, y1;
	uint16_t w, h;

	tft->setFont(&FreeMono9pt7b);
	tft->setCursor(x, y);
	tft->setTextColor(theme.getTempLabel());

	// Clear text area (maximum size)
	tft->getTextBounds(WSTATION_VERSION, x, y, &x1, &y1, &w, &h);
	tft->fillRect(x1, y1, w, h + 1, theme.getBackground());

	// Print version
	tft->setCursor(x, y);
	tft->print(WSTATION_VERSION);
}

/**
 * Draw all graphical elements in the screen
 */
void EInterface::showAll()
{
	int i;

	if (!state)
		return;

	showCity();
	showWeather(weather, period);
	showTempLabels();
	showTemp1(temp1);
	showTemp2(temp2);
	showChannel(channel);
	showHumidity1(humidity1);
	showHumidity2(humidity2);
	showRadio(radio);
	showIP();
	showWiFi(wifi);
	showBattery1(battery1);
	showBattery2(battery2);
	showClock(CLOCK_ALL);

	for (i = 0; i < 3; i++) {
		showForecastWeather(i, forecastWeather[i]);
		showForecastLabel(i, forecastLabels[i]);
		showForecastTemp1(i, forecastTemp1[i]);
		showForecastTemp2(i, forecastTemp2[i]);
	}
}

/**
 * Get temperature scale
 * @return temp_scale_t
 */
temp_scale_t EInterface::getTempScale()
{
	return tempScale;
}

/**
 * Get time format
 * @return time_format_t
 */
time_format_t EInterface::getTimeFormat()
{
	return timeFormat;
}

/**
 * Clear the whole screen
 */
void EInterface::clearAll()
{
	tft->fillScreen(theme.getBackground());
}

/**
 * Print text (at current cursor position, default color)
 * @param [in] text Text
 */
void EInterface::print(const String& text)
{
	this->print(-1, -1, -1, (int16_t)theme.getBackground(), text);
}

/**
 * Print text
 * @param [in] x X position
 * @param [in] y Y position
 * @param [in] text Text
 */
void EInterface::print(int x, int y, const String& text)
{
	print(x, y, -1, (int16_t)theme.getBackground(), text);
}

/**
 * Print text at current cursor position
 * @param [in] color Text color
 * @param [in] bgcolor Text background color (-1 for transparency)
 * @param [in] text Text
 */
void EInterface::print(int16_t color, int16_t bgcolor, const String& text)
{
	print(-1, -1, color, bgcolor, text);
}

/**
 * Print text
 * @param [in] x X position
 * @param [in] y Y position
 * @param [in] color Text color
 * @param [in] bgcolor Text background color (-1 for transparency)
 * @param [in] text Text
 */
void EInterface::print(int x, int y, int16_t color, int16_t bgcolor, const String& text)
{
	int16_t x1, y1;
	uint16_t w, h;

	// Default font
	tft->setFont(&FreeSans9pt7b);

	// Color
	if (color >= 0)
		tft->setTextColor(color);
	else
		tft->setTextColor(theme.getDefaultText());

	// Cursor position
	if (x >= 0 && y >= 0)
		tft->setCursor(x, y);

	/* Clear background area (when needed) */
	if (bgcolor >= 0) {
		tft->getTextBounds(text, x, y, &x1, &y1, &w, &h);
		tft->fillRect(x, y - h, w, h + 1, bgcolor);
	}

	tft->print(text);
}

#ifdef DEBUG_SCREENSHOT
/**
 * Take a screenshot of the screen
 * @param file File name
 */
void EInterface::takeScreenshot(String file)
{
	int16_t x, y, w, h;
	int pos;
	int imgsize;
	uint16_t *tft_buffer;
	uint16_t buffer[960]; // 240x4 tile
	File pic = pfs->open(file, "w");

	if (!pic)
		return;

	w = tft->width(); // 240
	h = tft->height(); // 320

	log_i("Taking screenshot %d x %d", w, h);
	pic.write((uint8_t*)&w, sizeof(w));
	pic.write((uint8_t*)&h, sizeof(h));

	// Take screenshot of the entire screen by 240x4 tiles (due to memory
	// limitation)
	for (y = 0; y < h; y+=4) {
		/* This loop takes longer, we need to refresh the watchdog */
		esp_task_wdt_reset();
		tft->readPixels16(0, y, w, 4, buffer);
		pic.write((uint8_t*)buffer, sizeof(buffer));
	}

	// Close file
	pic.close();
}
#endif

/* ======================= PRIVATE ======================= */

/**
 * Print a temperature value with degree symbol
 * @param [in] temp Temperature
 * @param [in] x X position
 * @param [in] y Y position
 */
void EInterface::drawTemp(float temp, int x, int y)
{
	char sc;
	char tempVal[12];
	int16_t x1, y1, dx, dy;
	uint16_t w, h;

	if (tempScale == CELSIUS) {
		sc = 'C';
	} else {
		sc = 'F';
	}
	
	if (temp == GUI_INV_TEMP) {
		snprintf(tempVal, sizeof(tempVal), "--.-  %c", sc);
	} else {
		snprintf(tempVal, sizeof(tempVal), "%.1f  %c", temp, sc);
	}

	tft->setFont(&FreeSansBold18pt7b);
	tft->setTextColor(theme.getTemperature());
	tft->setCursor(x, y);

	/* Clear background area (largest string) */
	tft->getTextBounds("-000.0 C", x, y, &x1, &y1, &w, &h);
	tft->fillRect(x, y - h, w, h + 1, theme.getBackground());

	/* Get the bounds of the current text */
	tft->getTextBounds(tempVal, x, y, &x1, &y1, &w, &h);
	dx = x + w - 30;
	dy = y - h + 5;

	/* Draw value and degree symbol */
	tft->print(tempVal);
	tft->drawCircle(dx, dy, 5, theme.getTemperature());
}

/**
 * Print a forecast temperature
 * @param [in] temp Temperature
 * @param [in] x X position
 * @param [in] y Y position
 * @param [in] color Color
 */
void EInterface::drawForecastTemp(float temp, int x, int y, int16_t color)
{
	char sc;
	char tempVal[12];
	int16_t x1, y1, dx, dy;
	uint16_t w, h;

	if (tempScale == CELSIUS) {
		sc = 'C';
	} else {
		sc = 'F';
	}

	if (temp == GUI_INV_TEMP) {
		snprintf(tempVal, sizeof(tempVal), "--.-  %c", sc);
	} else {
		snprintf(tempVal, sizeof(tempVal), "%.1f  %c", temp, sc);
	}

	tft->setFont(&FreeSans9pt7b);
	tft->setCursor(x, y);
	tft->setTextColor(color);

	/* Clear background area (largest string) */
	tft->getTextBounds("-000.0 C", x, y, &x1, &y1, &w, &h);
	tft->fillRect(x, y - h, w, h + 1, theme.getBackground());

	/* Get the bounds of the current text */
	tft->getTextBounds(tempVal, x, y, &x1, &y1, &w, &h);
	dx = x + w - 15;
	dy = y - h + 5;

	/* Draw value and degree symbol */
	tft->print(tempVal);
	tft->drawCircle(dx, dy, 3, color);
}

/**
 * Print humidity value
 * @param [in] humidity Humidity value (0 to 100)
 * @param [in] x X position
 * @param [in] y Y position
 */
void EInterface::drawHumidity(int humidity, int x, int y)
{
	char humVal[6];
	int16_t x1, y1;
	uint16_t w, h;

	tft->setFont(&FreeSansBold18pt7b);
	tft->setCursor(x, y);

	if (humidity == GUI_INV_HUMIDITY) {
		snprintf(humVal, sizeof(humVal), "--%%");
	} else {
		snprintf(humVal, sizeof(humVal), "%d%%", humidity);

		if (humidity >= HUMIDITY_L2_HIGH) {
			tft->setTextColor(theme.getHumidity(2));
		} else if (humidity >= HUMIDITY_L1_IDEAL) {
			tft->setTextColor(theme.getHumidity(1));
		} else {
			tft->setTextColor(theme.getHumidity(0));
		}
	}

	/* Clear background area (consider maximum size) */
	tft->getTextBounds("000%", x, y, &x1, &y1, &w, &h);
	tft->fillRect(x, y - h, w + 6, h + 1, theme.getBackground());

	/* Draw value */
	tft->print(humVal);
}

/**
 * Read 16 bits number from file
 * @param [in] f File
 * @return uint16_t
 */
uint16_t EInterface::readInt(File f)
{
	char num[2];
	num[0] = f.read();
	num[1] = f.read();
	return (((num[1] << 8) & 0xff00) | (num[0] & 0xff));
}

/**
 * Draw a pixel map file on the screen
 * @param [in] x Initial position (X axis)
 * @param [in] y Initial position (Y axis)
 * @param file File name
 */
void EInterface::drawPixmap(int x, int y, String file)
{
	int w, h;
	int pos;
	int imgsize;
	File pic = pfs->open(file, "r");

	if (!pic)
		return;

	w = readInt(pic);
	h = readInt(pic);
	imgsize = w * h;
	uint16_t *buffer = new uint16_t[imgsize];

	if (!buffer) {
		pic.close();
		return;
	}

	pos = 0;
	while(pic.available() && pos < imgsize) {
		buffer[pos++] = readInt(pic);
	}
	pic.close();

	tft->drawRGBBitmap(x, y, buffer, w, h);

	delete buffer;
}

/**
 * Draw a pixel map file at half size on the screen
 * @param [in] x Initial position (X axis)
 * @param [in] y Initial position (Y axis)
 * @param file File name
 */
void EInterface::drawPixmapHalf(int x, int y, String file)
{
	int w, h, wh, hh;
	int i, j;
	int pos;
	int imgsize;
	File pic = pfs->open(file, "r");

	if (!pic)
		return;

	w  = readInt(pic);
	h  = readInt(pic);
	wh = w / 2;
	hh = h / 2;
	imgsize = wh * hh;
	uint16_t buffer[imgsize];

	pos = 0;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j+=2) {
			if ((i % 2) != 0) {
				// Discard this line
				readInt(pic);
				readInt(pic);
				continue;
			} else {
				buffer[pos++] = readInt(pic);
				readInt(pic);
			}
		}
	}
	pic.close();

	tft->drawRGBBitmap(x, y, buffer, wh, hh);
}

/**
 * Convert weather type into the corresponding icon
 * @param [in] weather Weather ID
 * @param [in] Period: 0 = Day, 1 = Night
 * @return pixmap_t Corresponding icon type
 */
ETheme::pixmap_t EInterface::getWeatherIcon(weather_t weather, char period)
{
	ETheme::pixmap_t res;

	switch(weather) {
		case UNKNOWN_WEATHER:
			res = ETheme::FIG_UNKNOWN;
			break;
		case THUNDERSTORM_LIGHT_RAIN:
		case THUNDERSTORM_RAIN:
		case THUNDERSTORM_HEAVY_RAIN:
		case THUNDERSTORM_LIGHT:
		case THUNDERSTORM:
		case THUNDERSTORM_HEAVY:
		case THUNDERSTORM_RAGGED:
		case THUNDERSTORM_LIGHT_DRIZZLE:
		case THUNDERSTORM_DRIZZLE:
		case THUNDERSTORM_HEAVY_DRIZZLE:
			res = ETheme::FIG_11D;
			break;
		case DRIZZLE_LIGHT:
		case DRIZZLE:
		case DRIZZLE_HEAVY:
		case DRIZZLE_LIGHT_RAIN:
		case DRIZZLE_RAIN:
		case DRIZZLE_HEAVY_RAIN:
		case DRIZZLE_SHOWER_RAIN:
		case DRIZZLE_HEAVY_SHOWER_RAIN:
		case DRIZZLE_SHOWER:
			res = ETheme::FIG_09D;
			break;
		case RAIN_LIGHT:
		case RAIN_MODERATE:
		case RAIN_HEAVY:
		case RAIN_VERY_HEAVY:
		case RAIN_EXTREME:
			if (period == 0)
				res = ETheme::FIG_10D;
			else
				res = ETheme::FIG_10N;
			break;
		case RAIN_FREEZING:
			res = ETheme::FIG_13D;
			break;
		case RAIN_LIGHT_SHOWER:
		case RAIN_SHOWER:
		case RAIN_HEAVY_SHOWER:
		case RAIN_RAGGED:
			res = ETheme::FIG_09D;
			break;
		case SNOW_LIGHT:
		case SNOW:
		case SNOW_HEAVY:
		case SNOW_SLEET:
		case SNOW_LSHOWER_SLEET:
		case SNOW_SHOWER_SLEET:
		case SNOW_LIGHT_RAIN:
		case SNOW_RAIN:
		case SNOW_LIGHT_SHOWER:
		case SNOW_SHOWER:
		case SNOW_HEAVY_SHOWER:
			res = ETheme::FIG_13D;
			break;
		case ATM_MIST:
		case ATM_SMOKE:
		case ATM_HAZE:
		case ATM_DUST_WHIRLS:
		case ATM_FOG:
		case ATM_SAND:
		case ATM_DUST:
		case ATM_VOLCANIC_ASH:
		case ATM_SQUALL:
		case ATM_TORNADO:
			res = ETheme::FIG_50D;
			break;
		case CLEAR_SKY:
			if (period == 0)
				res = ETheme::FIG_01D;
			else
				res = ETheme::FIG_01N;
			break;
		case CLOUDS_FEW:
			if (period == 0)
				res = ETheme::FIG_02D;
			else
				res = ETheme::FIG_02N;
			break;
		case CLOUDS_SCATTERED:
			if (period == 0)
				res = ETheme::FIG_03D;
			else
				res = ETheme::FIG_03N;
			break;
		case CLOUDS_BROKEN:
		case CLOUDS_OVERCAST:
			if (period == 0)
				res = ETheme::FIG_04D;
			else
				res = ETheme::FIG_04N;
			break;
		default:
			res = ETheme::FIG_UNKNOWN;
	}

	return res;
}

/**
 * Get Celsius temperature converted (when needed)
 * @param [in] temp Temperature
 * @return float
 */
float EInterface::convCelsius(float temp)
{
	if (tempScale == FAHRENHEIT) {
		return (temp * 1.8) + 32;
	} else {
		return temp;
	}
}

