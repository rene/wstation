/* SPDX-License-Identifier: BSD-3-Clause */
/* 
 * Copyright (c) 2020 Renê de Souza Pinto. All rights reserverd.
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
 * @file webservices.h
 * \see webservices.cpp 
 */
#ifndef __WEB_SERVICES_H__
#define __WEB_SERVICES_H__

#include <ESPAsyncWebServer.h>
#include "UserConf.h"

/* HTML form fields */
#define PARAM_SSID     "ssid"
#define PARAM_WIFIPASS "wifipass"
#define PARAM_KEY      "key"
#define PARAM_CITY     "city"
#define PARAM_DATE     "date"
#define PARAM_TIMEZONE "tz"
#define PARAM_DAYLIGHT "dayl"
#define PARAM_HOURS    "hours"
#define PARAM_MINUTES  "minutes"
#define PARAM_SECONDS  "seconds"
#define PARAM_NTP      "ntp"
#define PARAM_LCDBRIG  "brightness"
#define PARAM_TEMPSCALE "tempscale"
#define PARAM_USERNAME  "username"
#define PARAM_USER_PASS "userpass"
#define PARAM_TIME_FMT  "timeformat"

/* Reset mutex */
extern volatile SemaphoreHandle_t reset_mutex;
/* User configuration */
extern UserConf confData;

/* Setup all web services */
void SetupWebServices(AsyncWebServer *webServer);

#endif /* __WEB_SERVICES_H__ */
