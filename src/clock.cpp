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
 * @file clock.cpp
 * Function to retrieve time from different sources
 */
#include "clock.h"

#ifdef RTC_DS1307
/**
 * Read time from RTC
 * @param [out] _tm Date and Time
 * @return int
 */
int readClock(tmElements_t *_tm)
{
	int res = 0;
	if (!RTC.read(*_tm)) {
		res = -1;
		if (RTC.chipPresent()) {
			_tm->Day    = 1;
			_tm->Month  = 1;
			_tm->Year   = CalendarYrToTm(2020);
			_tm->Hour   = 0;
			_tm->Minute = 0;
			_tm->Second = 0;
			RTC.write(*_tm);
		}
	}
	return res;
}

/**
 * Save time
 * @param [in] _tm Date and Time
 * @return int
 */
int writeClock(tmElements_t *_tm)
{
	RTC.write(*_tm);
	return 0;
}
#else
/**
 * Read time
 * @param [out] _tm Date and Time
 * @return int
 */
int readClock(tmElements_t *_tm)
{
	getSysClock(_tm);
	return 0;
}

/**
 * Save time
 * @param [in] _tm Date and Time
 * @return int
 */
int writeClock(tmElements_t *_tm)
{
	struct timeval tv;
	struct timeval tv_now;
	struct tm ftime;

	// Get current time
	gettimeofday(&tv_now, NULL);

	ftime.tm_hour = _tm->Hour;
	ftime.tm_min  = _tm->Minute;
	ftime.tm_sec  = _tm->Second;
	ftime.tm_mday = _tm->Day;
	ftime.tm_mon  = _tm->Month - 1;
	ftime.tm_wday = _tm->Wday;
	ftime.tm_year = tmYearToCalendar(_tm->Year) - 1900;

	tv.tv_sec  = mktime(&ftime);
	tv.tv_usec = 0;

	// Adjust time
	return settimeofday(&tv, NULL);
}
#endif

/**
 * Get system's clock
 * @param [out] _tm System clock
 */
void getSysClock(tmElements_t *_tm)
{
	struct timeval tv;
	struct tm *ftime;

	// Get date and time
	gettimeofday(&tv, NULL);

	// Format data
	ftime = localtime(&tv.tv_sec);

	_tm->Hour   = ftime->tm_hour;
	_tm->Minute = ftime->tm_min;
	_tm->Second = ftime->tm_sec;
	_tm->Day    = ftime->tm_mday;
	_tm->Month  = ftime->tm_mon + 1;
	_tm->Wday   = ftime->tm_wday + 1;
	_tm->Year   = CalendarYrToTm(ftime->tm_year + 1900);
}

