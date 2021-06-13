/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2021 Renê de Souza Pinto
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
 * @file HTU2XSensor.h
 */
#ifndef __HTU2XSENSOR_H__
#define __HTU2XSENSOR_H___

#include <SparkFunHTU21D.h>
#include "wstation.h"
#include "THSensor.h"

/**
 * @class HTU2XSensor
 * THSensor class implementation for HTU2X Sensors
 */
class HTU2XSensor : public THSensor {
	private:
		HTU21D htuSensor;

	public:
		/**
		 * Setup sensor
		 * @return int 0 on success, error code otherwise
		 */
		int setup() {
			htuSensor.begin();
			return 0;
		}

		/**
		 * Read sensor's data
		 * @return int 0 on success, error code otherwise
		 */
		int readSensor() {
			float t = htuSensor.readTemperature();
			float h = htuSensor.readHumidity();

			if ((h == ERROR_I2C_TIMEOUT || h == ERROR_BAD_CRC) ||
					(t == ERROR_I2C_TIMEOUT || t == ERROR_BAD_CRC)) {
				return -1;
			} else {
				temperature = t;
				humidity    = h;
			}

			return 0;
		}
};

#endif /* __HTU2XSENSOR_H__ */
