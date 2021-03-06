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
 * @file UserConf.cpp
 * @class UserConf
 * Handle user configuration data
 */
#include <UserConf.h>
#include <ArduinoNvs.h>
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
	temp_scale_t tempScale;
	char username[MAX_STR_SIZE];
	char userpass[MAX_STR_SIZE];
	time_format_t timeFormat;
} __attribute__((packed));

/** User configuration data at EEPROM */
typedef struct _user_conf user_conf_t;

/**
 * Constructor
 */
UserConf::UserConf() :
	confStatus(0), wifiSSID(DEFCONF_WIFI_SSID),
	wifiPassword(DEFCONF_WIFI_PASS), owKey(DEFCONF_OW_KEY),
	owCity(DEFCONF_OW_CITY), ntpServer(DEFCONF_NTPSERVER),
	hours(DEFCONF_HOURS), minutes(DEFCONF_MINUTES),
	timezone(DEFCONF_TIMEZONE), daylight(DEFCONF_DAYLIGHT),
	seconds(DEFCONF_SECONDS), day(DEFCONF_DAY), month(DEFCONF_MONTH),
	year(DEFCONF_YEAR), brightness(DEFCONF_BRIGHTNESS),
	tempScale(DEFCONF_TEMP_SCALE),
	username(DEFCONF_USERNAME), userpass(DEFCONF_USER_PASS),
	timeFormat(DEFCONF_TIME_FORMAT)
{
}

/**
 * Wireless network: SSID
 * @param [in] ssid Network SSID
 */
void UserConf::setWiFiSSID(const String& ssid)
{
	wifiSSID = ssid;
}

/**
 * Wireless network: Password
 * @param [in] password Network password
 */
void UserConf::setWiFiPassword(const String& password)
{
	wifiPassword = password;
}

/**
 * Return wireless network SSID
 * @return String
 */
const String& UserConf::getWiFiSSID()
{
	return wifiSSID;
}

/**
 * Return wireless network password
 * @return String
 */
const String& UserConf::getWiFiPassword()
{
	return wifiPassword;
}

/**
 * Openweather: API key
 * @param [in] key API key
 */
void UserConf::setAPIKey(const String& key)
{
	owKey = key;
}

/**
 * Get API key
 * @return String
 */
const String& UserConf::getAPIKey()
{
	return owKey;
}

/**
 * Openweather: City name
 * @param [in] city City name in Openweather format. For instance: Berlin,DE
 */
void UserConf::setCity(const String& city)
{
	owCity = city;
}

/**
 * Get city name
 * @return String
 */
const String& UserConf::getCity()
{
	return owCity;
}

/**
 * Set NTP server
 * @param [in] ntpServer NTP Server
 */
void UserConf::setNTPServer(const String& ntpServer)
{
	this->ntpServer = ntpServer;
}

/**
 * Get NTP server
 * @return String
 */
const String& UserConf::getNTPServer()
{
	return ntpServer;
}

/**
 * Set hours
 * @param [in] hours Hours
 */
void UserConf::setHours(int hours)
{
	if (hours >= 0 && hours <= 23) {
		this->hours = hours;
	}
}

/**
 * Set minutes
 * @param [in] minutes Minutes
 */
void UserConf::setMinutes(int minutes)
{
	if (minutes >= 0 && minutes <= 59) {
		this->minutes = minutes;
	}
}

/**
 * Set seconds
 * @param [in] seconds Seconds
 */
void UserConf::setSeconds(int seconds)
{
	if (seconds >= 0 && seconds <= 59) {
		this->seconds = seconds;
	}
}

/**
 * Timezone offset
 * @param [in] offset
 */
void UserConf::setTimezone(int offset)
{
	timezone = offset;
}

/**
 * Daylight offset
 * @param [in] offset
 */
void UserConf::setDaylight(int offset)
{
	daylight = offset;
}

/**
 * Set temperature scale
 * @param [in] scale CELSIUS or FAHRENHEIT
 */
void UserConf::setTempScale(temp_scale_t scale)
{
	tempScale = scale;
}

/**
 * Set username
 * @param [in] username Username
 */
void UserConf::setUsername(const String& username)
{
	this->username = username;
}

/**
 * Set user password
 * @param [in] userpass User password
 */
void UserConf::setUserPass(const String& userpass)
{
	this->userpass = userpass;
}

/**
 * Set time format
 * @param [in] timeFormat TIME_FORMAT_24H or TIME_FORMAT_12H
 */
void UserConf::setTimeFormat(time_format_t timeFormat)
{
	this->timeFormat = timeFormat;
}

/**
 * Get hours
 * @return int
 */
int UserConf::getHours()
{
	return hours;
}

/**
 * Get minutes
 * @return int
 */
int UserConf::getMinutes()
{
	return minutes;
}

/**
 * Get seconds
 * @return int
 */
int UserConf::getSeconds()
{
	return seconds;
}

/**
 * Get timezone offset
 * @return int Timezone offset (seconds)
 */
int UserConf::getTimezone()
{
	return timezone;
}

/**
 * Get daylight offset
 * @return int Daylight offset (seconds)
 */
int UserConf::getDaylight()
{
	return daylight;
}

/**
 * Get temperature scale
 * @return temp_scale_t
 */
temp_scale_t UserConf::getTempScale()
{
	return tempScale;
}

/**
 * Get username
 * @return String
 */
const String& UserConf::getUsername()
{
	return username;
}

/**
 * Get user password
 * @return String
 */
const String& UserConf::getUserPass()
{
	return userpass;
}

/**
 * Get time format
 * @return time_format_t
 */
time_format_t UserConf::getTimeFormat()
{
	return timeFormat;
}

/**
 * Set date
 * @param [in] day Day
 * @param [in] month Month
 * @param [in] year Year
 * @param [in] wday Week day
 */
void UserConf::setDate(int day, int month, int year, int wday)
{
	this->day   = day;
	this->month = month;
	this->year  = year;
	this->wday  = wday;
}

/**
 * Get date
 * @param [out] day Day
 * @param [out] month Month
 * @param [out] year Year
 * @param [out] wday Week day
 */
void UserConf::getDate(int *day, int *month, int *year, int *wday)
{
	*day   = this->day;
	*month = this->month;
	*year  = this->year;
	*wday  = this->wday;
}

/**
 * Set LCD brightness
 * @param [in] brightness Brightness
 */
void UserConf::setLCDBrightness(int brightness)
{
	if (brightness > 0) {
		this->brightness = brightness;
	}
}

/**
 * Get LCD brightness
 * @return int
 */
int UserConf::getLCDBrightness()
{
	return brightness;
}

/**
 * Return true if user has configured the system
 * @return bool True if user has set up options, false otherwise
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
 * Save all configuration data to EEPROM
 */
void UserConf::SaveConf()
{
	SaveConf(1);
}

/**
 * Save all configuration data to EEPROM
 * @param [in] char Configuration status
 */
void UserConf::SaveConf(char confStatus)
{
	int addr;
	user_conf_t uconf;

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
	uconf.tempScale  = tempScale;
	strncpy(uconf.username, username.c_str(), MAX_STR_SIZE);
	strncpy(uconf.userpass, userpass.c_str(), MAX_STR_SIZE);
	uconf.timeFormat = timeFormat;

	// Write struct to EEPROM
	NVS.setBlob("uconf", (uint8_t*)&uconf, sizeof(uconf));
}

/**
 * Read all configuration data from EEPROM
 */
void UserConf::ReadConf()
{
	int addr;
	bool res;
	user_conf_t uconf;

	// Read from EEPROM
	res = NVS.getBlob("uconf", (uint8_t*)&uconf, sizeof(uconf));
	if (!res) {
		ResetConf();
		return;
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
	tempScale    = uconf.tempScale;
	username     = String(uconf.username);
	userpass     = String(uconf.userpass);
	timeFormat   = uconf.timeFormat;
}

/**
 * Reset configuration to default settings
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
	tempScale    = DEFCONF_TEMP_SCALE;
	username     = DEFCONF_USERNAME;
	userpass     = DEFCONF_USER_PASS;
	timeFormat   = DEFCONF_TIME_FORMAT;
	confStatus   = 0;

	// Save to EEPROM
	SaveConf(0);
}

