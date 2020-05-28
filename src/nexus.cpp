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
 * \file: nexus.c
 * \brief Provide functions to handle NC-7345 sensor data
 */

#include <Arduino.h>
#include "nexus.h"

/** Number of received frames in the buffer */
#define FBUFF_SIZE 3

/** Data received */
volatile nexus_t nexusData;
/** Data received indicator */
volatile bool nexusDataAvailable;

/** Time record for each interrupt */
static volatile unsigned long pTime[2];
/** Data frame */
static volatile uint64_t frame;
/** How many bits has been received */
static volatile int bcnt;
/** Last received frames */
static volatile uint64_t frames[FBUFF_SIZE];
/** Current buffer position */
static volatile char fpos;
/** Mutex */
portMUX_TYPE nexusMutex = portMUX_INITIALIZER_UNLOCKED;

/**
 * \brief Retrieve an interval of bits from a frame
 * \param [in] frm Data frame
 * \param [in] first Position of the first bit in the interval
 * \param [in] size Number of bits
 * \return uint64_t Selected bits
 */
static uint64_t IRAM_ATTR getBits(uint64_t frm, int first, int size)
{
	uint64_t mask = (1ULL << size) - 1;
	uint64_t res  = (frm & (mask << first)) >> first;
	return res;
}

/**
 * \brief Parse frames in the buffer
 */
static void IRAM_ATTR parseFrames()
{
	int i;
	uint64_t frm;
	char check;
	nexus_t info;

	/* All frames in the buffer should be equal, indicating that probably we
	 * have received valid frames */
	frm = frames[0];
	for (i = 1, check = 1; i < FBUFF_SIZE; i++) {
		if (frm != frames[i]) {
			check = 0;
			break;
		}
	}

	if (!check)
		return;

	/* Frames are equal, check const value */
	info._const = (uint8_t)getBits(frm, 8, 4);
	if (info._const != 0x0F)
		return;

	/* Frame is valid, parse */
	info.id          = (uint8_t)getBits(frm, 28, 8);
	info.flags       = (uint8_t)getBits(frm, 24, 4);
	info.temperature = (int16_t)((uint16_t)getBits(frm, 12, 12) << 4);
	info.temperature = info.temperature >> 4;
	info.humidity    = (uint8_t)getBits(frm,  0, 8);

	portENTER_CRITICAL_ISR(&nexusMutex);
	nexusData.id          = info.id;
	nexusData.flags       = info.flags;
	nexusData.temperature = info.temperature;
	nexusData.humidity    = info.humidity;
	nexusDataAvailable = true;
	portEXIT_CRITICAL_ISR(&nexusMutex);
}

/**
 * \brief Parse sensor signal pulse
 * \note This function should be executed on each interrupt signal
 *
 * NC-7345 sensors use PPM (Pulse-Position Modulation)
 *
 * Start frame:         ~4000us
 * Bit 0 (short pulse): ~3000us
 * Bit 1 (long pulse):  ~2000us
 *
 * 36 bits/frame x 12 frames
 *
 * Frame format:
 *   Size (bits):  8     4      12      4         8
 *   Field:      [ID] [Flags] [TEMP] [const] [Humidity]
 *
 * ID (8 bits): Sensor ID
 *
 * Flags (4 bits): [B] 0 [C] [C]
 *       B (Battery level): 1 = Good, 0 = Low
 *       CC (Channel): 00 = CH1, 01 = CH2, 10 = CH3
 *
 * TEMP: 12 bits signed integer scaled by 10
 *
 * const: Should be always 0x0F (1111)
 *
 * Humidity: 8 bits (value is humidity percentage)
 *
 * PS: Frame information from RTL_433 project:
 * https://github.com/merbanan/rtl_433.git
 */
void IRAM_ATTR nexusHandlePulse()
{
	unsigned long dt;

	/* Get time */
	pTime[1] = micros();

	/* Calculate time delta */
	dt       = pTime[1] - pTime[0];
	pTime[0] = pTime[1];

	/* Detect start frame (sync pulse) */
	if (dt >= 3600) {
		bcnt  = 0;
		frame = 0;
		return;
	}

	/* Detect pulse */
	if (dt >= 1400 && dt < 2400) {
		/* Short pulse detected */
		frame  = frame << 1;
		frame &= 0xffffffffe;
		bcnt++;
	} else if (dt >= 2400 && dt < 3600) {
		/* Long pulse detected */
		frame  = frame << 1;
		frame |= 0x1;
			bcnt++;
	}

	/* Check frame completion */
	if (bcnt >= 36) {
		bcnt = 0;
		frames[fpos++] = frame;
		if (fpos >= FBUFF_SIZE) {
			fpos = 0;
			parseFrames();
		}
	}
}

/**
 * \brief Setup interrupt handler
 * \param [in] pin Pin where receiver is attached
 */
void setupNexus(int pin)
{
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pin), nexusHandlePulse, FALLING);

	nexusDataAvailable = false;

	pTime[0] = micros();
	frame    = 0;
	bcnt     = 0;
	fpos     = 0;
}

