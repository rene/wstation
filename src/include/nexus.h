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
 * @file nexus.h
 * \see nexus.c
 */
#ifndef __NEXUS_H__
#define __NEXUS_H__

/** Channel 1 */
#define NEXUS_CHANNEL_1 0x0
/** Channel 2 */
#define NEXUS_CHANNEL_2 0x1
/** Channel 3 */
#define NEXUS_CHANNEL_3 0x2
/** Invalid channel */
#define NEXUS_INVALID_CHANNEL 0x03

/** Sensor's flags */
typedef union {
	struct _fields {
		uint8_t channel : 2;
		uint8_t reserved: 1;
		uint8_t battery : 1;
		uint8_t padding : 4;
	} fields;
	uint8_t raw;
} nexus_flags_t;

/** Sensor data structure */
typedef struct _nexus_t {
	/** ID */
	uint8_t id;
	/** Flags: B 0 CH CH - Battery status (0=LOW, 1=OK), CH CH = Channel */
	nexus_flags_t flags;
	/** Temperature (x10), 12 bits */
	int16_t temperature;
	/** Constant: should be 0xff */
	uint8_t  _const;
	/** Humidity */
	uint8_t humidity;
} nexus_t;

/* Data received */
extern volatile nexus_t nexusData;
/* Data received indicator */
extern volatile bool nexusDataAvailable;
/* Mutex to access data */
extern portMUX_TYPE nexusMutex;

/* Setup interrupt handler */
void setupNexus(int pin);

/* Interrupt handler */
void nexusHandlePulse();

#endif /* __NEXUS_H__ */
