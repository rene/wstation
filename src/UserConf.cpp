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
 * \file UserConf.cpp
 * \class UserConf
 * \brief Handle user configuration data
 */
#include <UserConf.h>
#include <EEPROM.h>
#include <string.h>

/** Maximum string length for configuration attributes */
#define MAX_STR_SIZE 64

/** Layout for user configuration data at EEPROM */
struct _user_conf {
	char confStatus;
	char wifiSSID[MAX_STR_SIZE];
	char wifiPassword[MAX_STR_SIZE];
	char owKey[MAX_STR_SIZE];
	char owCity[MAX_STR_SIZE];
	char ntpServer[MAX_STR_SIZE];
	int hours;
	int minutes;
	int seconds;
	int timezone;
	int daylight;
	int day;
	int month;
	int year;
	int wday;
	int brightness;
} __attribute__((packed));

/** User configuration data at EEPROM */
typedef struct _user_conf user_conf_t;

/**
 * \brief Constructor
 */
UserConf::UserConf() :
	confStatus(0), wifiSSID(DEFCONF_WIFI_SSID),
	wifiPassword(DEFCONF_WIFI_PASS), owKey(DEFCONF_OW_KEY),
	owCity(DEFCONF_OW_CITY), ntpServer(DEFCONF_NTPSERVER),
	hours(DEFCONF_HOURS), minutes(DEFCONF_MINUTES),
	timezone(DEFCONF_TIMEZONE), daylight(DEFCONF_DAYLIGHT),
	seconds(DEFCONF_SECONDS), day(DEFCONF_DAY), month(DEFCONF_MONTH),
	year(DEFCONF_YEAR), brightness(DEFCONF_BRIGHTNESS)
{
}

/**
 * \brief Wireless network: SSID
 * \param [in] ssid Network SSID
 */
void UserConf::setWiFiSSID(const String& ssid)
{
	wifiSSID = ssid;
}

/**
 * \brief Wireless network: Password
 * \param [in] password Network password
 */
void UserConf::setWiFiPassword(const String& password)
{
	wifiPassword = password;
}

/**
 * \brief Return wireless network SSID
 * \return String
 */
const String& UserConf::getWiFiSSID()
{
	return wifiSSID;
}

/**
 * \brief Return wireless network password
 * \return String
 */
const String& UserConf::getWiFiPassword()
{
	return wifiPassword;
}

/**
 * \brief Openweather: API key
 * \param [in] key API key
 */
void UserConf::setAPIKey(const String& key)
{
	owKey = key;
}

/**
 * \brief Get API key
 * \return String
 */
const String& UserConf::getAPIKey()
{
	return owKey;
}

/**
 * \brief Openweather: City name
 * \param [in] city City name in Openweather format. For instance: Berlin,DE
 */
void UserConf::setCity(const String& city)
{
	owCity = city;
}

/**
 * \brief Get city name
 * \return String
 */
const String& UserConf::getCity()
{
	return owCity;
}

/**
 * \brief Set NTP server
 * \param [in] ntpServer NTP Server
 */
void UserConf::setNTPServer(const String& ntpServer)
{
	this->ntpServer = ntpServer;
}

/**
 * \brief Get NTP server
 * \return String
 */
const String& UserConf::getNTPServer()
{
	return ntpServer;
}

/**
 * \brief Set hours
 * \param [in] hours Hours
 */
void UserConf::setHours(int hours)
{
	if (hours >= 0 && hours <= 23) {
		this->hours = hours;
	}
}

/**
 * \brief Set minutes
 * \param [in] minutes Minutes
 */
void UserConf::setMinutes(int minutes)
{
	if (minutes >= 0 && minutes <= 59) {
		this->minutes = minutes;
	}
}

/**
 * \brief Set seconds
 * \param [in] seconds Seconds
 */
void UserConf::setSeconds(int seconds)
{
	if (seconds >= 0 && seconds <= 59) {
		this->seconds = seconds;
	}
}

/**
 * \brief Timezone offset
 * \param [in] offset
 */
void UserConf::setTimezone(int offset)
{
	timezone = offset;
}

/**
 * \brief Daylight offset
 * \param [in] offset
 */
void UserConf::setDaylight(int offset)
{
	daylight = offset;
}

/**
 * \brief Get hours
 * \return int
 */
int UserConf::getHours()
{
	return hours;
}

/**
 * \brief Get minutes
 * \return int
 */
int UserConf::getMinutes()
{
	return minutes;
}

/**
 * \brief Get seconds
 * \return int
 */
int UserConf::getSeconds()
{
	return seconds;
}

/**
 * \brief Get timezone offset
 * \return int Timezone offset (seconds)
 */
int UserConf::getTimezone()
{
	return timezone;
}

/**
 * \brief Get daylight offset
 * \return int Daylight offset (seconds)
 */
int UserConf::getDaylight()
{
	return daylight;
}

/**
 * \brief Set date
 * \param [in] day Day
 * \param [in] month Month
 * \param [in] year Year
 * \param [in] wday Week day
 */
void UserConf::setDate(int day, int month, int year, int wday)
{
	this->day   = day;
	this->month = month;
	this->year  = year;
	this->wday  = wday;
}

/**
 * \brief Get date
 * \param [out] day Day
 * \param [out] month Month
 * \param [out] year Year
 * \param [out] wday Week day
 */
void UserConf::getDate(int *day, int *month, int *year, int *wday)
{
	*day   = this->day;
	*month = this->month;
	*year  = this->year;
	*wday  = this->wday;
}

/**
 * \brief Set LCD brightness
 * \param [in] brightness Brightness
 */
void UserConf::setLCDBrightness(int brightness)
{
	if (brightness > 0) {
		this->brightness = brightness;
	}
}

/**
 * \brief Get LCD brightness
 * \return int
 */
int UserConf::getLCDBrightness()
{
	return brightness;
}

/**
 * \brief Return true if user has configured the system
 * \return bool True if user has set up options, false otherwise
 */
bool UserConf::isConfigured()
{
	if (confStatus == 1) {
		return true;
	} else {
		return false;
	}
}

/**
 * \brief Save all configuration data to EEPROM
 */
void UserConf::SaveConf()
{
	SaveConf(1);
}

/**
 * \brief Save all configuration data to EEPROM
 * \param [in] char Configuration status
 */
void UserConf::SaveConf(char confStatus)
{
	int addr;
	user_conf_t uconf;
	char *ptr = (char *)&uconf;

	// Change status
	this->confStatus = confStatus;

	// Fill struct with data
	strncpy(uconf.wifiSSID,     wifiSSID.c_str(),     MAX_STR_SIZE);
	strncpy(uconf.wifiPassword, wifiPassword.c_str(), MAX_STR_SIZE);
	strncpy(uconf.owKey,        owKey.c_str(),        MAX_STR_SIZE);
	strncpy(uconf.owCity,       owCity.c_str(),       MAX_STR_SIZE);
	strncpy(uconf.ntpServer,    ntpServer.c_str(),    MAX_STR_SIZE);
	uconf.hours      = hours;
	uconf.minutes    = minutes;
	uconf.seconds    = seconds;
	uconf.timezone   = timezone;
	uconf.daylight   = daylight;
	uconf.day        = day;
	uconf.month      = month;
	uconf.year       = year;
	uconf.wday       = wday;
	uconf.brightness = brightness;
	uconf.confStatus = this->confStatus;

	// Write struct to EEPROM
	for (addr = 0; addr < sizeof(uconf); addr++) {
		EEPROM.write(addr, ptr[addr]);
	}
}

/**
 * \brief Read all configuration data from EEPROM
 */
void UserConf::ReadConf()
{
	int addr;
	user_conf_t uconf;
	char *ptr = (char *)&uconf;

	// Read from EEPROM
	for (addr = 0; addr < sizeof(uconf); addr++) {
		ptr[addr] = EEPROM.read(addr);
	}

	// Fill attributes
	wifiSSID     = String(uconf.wifiSSID);
	wifiPassword = String(uconf.wifiPassword);
	owKey        = String(uconf.owKey);
	owCity       = String(uconf.owCity);
	ntpServer    = String(uconf.ntpServer);
	hours        = uconf.hours;
	minutes      = uconf.minutes;
	seconds      = uconf.seconds;
	timezone     = uconf.timezone;
	daylight     = uconf.daylight;
	day          = uconf.day;
	month        = uconf.month;
	year         = uconf.year;
	wday         = uconf.wday;
	brightness   = uconf.brightness;
	confStatus   = uconf.confStatus;
}

/**
 * \brief Reset configuration to default settings
 */
void UserConf::ResetConf()
{
	// Fill attributes
	wifiSSID     = DEFCONF_WIFI_SSID;
	wifiPassword = DEFCONF_WIFI_PASS;
	owKey        = DEFCONF_OW_KEY;
	owCity       = DEFCONF_OW_CITY;
	ntpServer    = DEFCONF_NTPSERVER;
	hours        = DEFCONF_HOURS;
	minutes      = DEFCONF_MINUTES;
	seconds      = DEFCONF_SECONDS;
	timezone     = DEFCONF_TIMEZONE;
	daylight     = DEFCONF_DAYLIGHT;
	day          = DEFCONF_DAY;
	month        = DEFCONF_MONTH;
	year         = DEFCONF_YEAR;
	wday         = DEFCONF_WDAY;
	brightness   = DEFCONF_BRIGHTNESS;
	confStatus   = 0;

	// Save to EEPROM
	SaveConf(0);
}

