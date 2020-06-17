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
 * \file UserConf.h
 * \see UserConf.cpp
 */
#ifndef USERCONF_H
#define USERCONF_H

#include <Arduino.h>

class UserConf {
	private:
		/** Wireless network SSID */
		String wifiSSID;
		/** Wireless network password */
		String wifiPassword;
		/** Openweather API key */
		String owKey;
		/** Openweather city */
		String owCity;
		/** Hours */
		int hours;
		/** Minutes */
		int minutes;
		/** Seconds */
		int seconds;
		/** LCD brightness */
		int brightness;

	public:
		/* Wireless network: SSID */
		void setWiFiSSID(const String& ssid);
		/* Wireless network: Password */
		void setWiFiPassword(const String& ssid);
		/* Return wireless network SSID */
		const String& getWiFiSSID();
		/* Return wireless network password */
		const String& getWiFiPassword();
		/* Openweather: API key */
		void setAPIKey(const String& key);
		/* Openweather: City name */
		void setCity(const String& city);
		/* Set hours */
		void setHours(int hours);
		/* Set minutes */
		void setMinutes(int minutes);
		/* Set seconds */
		void setSeconds(int seconds);
		/* Get hours */
		int getHours();
		/* Get minutes */
		int getMinutes();
		/* Get seconds */
		int getSeconds();
		/* Set LCD brightness */
		void setLCDBrightness(int brightness);
		/* Get LCD brightness */
		int getLCDBrightness();
		/* Save all configuration data to EEPROM */
		void SaveConf();
		/* Read all configuration data from EEPROM */
		void ReadConf();
};
#endif /* USERCONF_H */
