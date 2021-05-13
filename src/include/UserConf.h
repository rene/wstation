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
 * @file UserConf.h
 * \see UserConf.cpp
 */
#ifndef USERCONF_H
#define USERCONF_H

#include <Arduino.h>
#include "EInterface.h"
#include "wstation.h"

/** Default value for: WiFi SSID */
#define DEFCONF_WIFI_SSID  DEFAULT_AP_SSID
/** Default value for: WiFi password */
#define DEFCONF_WIFI_PASS  DEFAULT_AP_PASS
/** Default value for: Username */
#define DEFCONF_USERNAME   DEFAULT_USERNAME
/** Default value for: User password */
#define DEFCONF_USER_PASS  DEFAULT_USER_PASS
/** Default value for: Openweather API key */
#define DEFCONF_OW_KEY     ""
/** Default value for: Openweather city */
#define DEFCONF_OW_CITY    "Berlin,DE"
/** Default value for: NTP server */
#define DEFCONF_NTPSERVER  "pool.ntp.org"
/** Default value for: hours */
#define DEFCONF_HOURS      0
/** Default value for: minutes */
#define DEFCONF_MINUTES    0
/** Default value for: seconds */
#define DEFCONF_SECONDS    0
/** Default timezone */
#define DEFCONF_TIMEZONE   3600
/** Default daylight offset */
#define DEFCONF_DAYLIGHT   3600
/** Default value for: day */
#define DEFCONF_DAY        10
/** Default value for: month */
#define DEFCONF_MONTH      3
/** Default value for: year */
#define DEFCONF_YEAR       2020
/** Default value for: week day */
#define DEFCONF_WDAY       3
/** Default value for: day */
#define DEFCONF_BRIGHTNESS BACKLIGHT_DEFAULT
/** Default value for: temperature scale */
#define DEFCONF_TEMP_SCALE CELSIUS
/** Default value for: time format */
#define DEFCONF_TIME_FORMAT TIME_FORMAT_24H


class UserConf {
	private:
		/** Configuration status */
		char confStatus;
		/** Wireless network SSID */
		String wifiSSID;
		/** Wireless network password */
		String wifiPassword;
		/** Openweather API key */
		String owKey;
		/** Openweather city */
		String owCity;
		/** NTP server */
		String ntpServer;
		/** Hours */
		int hours;
		/** Minutes */
		int minutes;
		/** Seconds */
		int seconds;
		/** Timezone offset */
		int timezone;
		/** Daylight offset */
		int daylight;
		/** Day */
		int day;
		/** Month */
		int month;
		/** Year */
		int year;
		/** Week day */
		int wday;
		/** LCD brightness */
		int brightness;
		/** Temperature scale */
		temp_scale_t tempScale;
		/** Username */
		String username;
		/** User password */
		String userpass;
		/** Time format */
		time_format_t timeFormat;

	public:
		/* Constructor */
		UserConf();
		/* Wireless network: SSID */
		void setWiFiSSID(const String& ssid);
		/* Wireless network: Password */
		void setWiFiPassword(const String& password);
		/* Return wireless network SSID */
		const String& getWiFiSSID();
		/* Return wireless network password */
		const String& getWiFiPassword();
		/* Openweather: API key */
		void setAPIKey(const String& key);
		/* Get API key */
		const String& getAPIKey();
		/* Openweather: City name */
		void setCity(const String& city);
		/* Get city name */
		const String& getCity();
		/* Set NTP server */
		void setNTPServer(const String& ntpServer);
		/* Get NTP server */
		const String& getNTPServer();
		/* Set hours */
		void setHours(int hours);
		/* Set minutes */
		void setMinutes(int minutes);
		/* Set seconds */
		void setSeconds(int seconds);
		/* Timezone offset */
		void setTimezone(int offset);
		/* Daylight offset */
		void setDaylight(int offset);
		/* Set temperature scale */
		void setTempScale(temp_scale_t scale);
		/* Set username */
		void setUsername(const String& username);
		/* Set user password */
		void setUserPass(const String& userpass);
		/* Set time format */
		void setTimeFormat(time_format_t timeFormat);
		/* Get hours */
		int getHours();
		/* Get minutes */
		int getMinutes();
		/* Get seconds */
		int getSeconds();
		/* Get timezone offset */
		int getTimezone();
		/* Get daylight offset */
		int getDaylight();
		/* Get temperature scale */
		temp_scale_t getTempScale();
		/* Get username */
		const String& getUsername();
		/* Get user password */
		const String& getUserPass();
		/* Get time format */
		time_format_t getTimeFormat();
		/* Set date */
		void setDate(int day, int month, int year, int wday);
		/* Get date */
		void getDate(int *day, int *month, int *year, int *wday);
		/* Set LCD brightness */
		void setLCDBrightness(int brightness);
		/* Get LCD brightness */
		int getLCDBrightness();
		/* Return true if user has configured the system */
		bool isConfigured();
		/* Save all configuration data to EEPROM */
		void SaveConf();
		/* Save all configuration data to EEPROM */
		void SaveConf(char confStatus);
		/* Read all configuration data from EEPROM */
		void ReadConf();
		/* Reset configuration to default settings */
		void ResetConf();
};
#endif /* USERCONF_H */
