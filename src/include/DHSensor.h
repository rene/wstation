/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2021 Renê de Souza Pinto. All rights reserverd.
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
 * @file DHTSensor.h
 */
#ifndef __DHTSENSOR_H__
#define __DHTSENSOR_H___

#include <DHTesp.h>
#include "wstation.h"
#include "THSensor.h"

/**
 * @class DHTSensor
 * THSensor class implementation for DHT Sensor
 */
class DHTSensor : public THSensor {
	private:
		DHTesp dhtSensor;
		TempAndHumidity sensorData;

	public:
		/**
		 * Setup sensor
		 * @return int 0 on success, error code otherwise
		 */
		int setup() {
			dhtSensor.setup(DHT_DATA_PIN, DHTesp::AUTO_DETECT);
			return 0;
		}

		/**
		 * Return sensor's status
		 * @return int Status code
		 */
		int getStatus() {
			return dhtSensor.getStatus();
		}

		/**
		 * Return sensor's status string
		 * @return String Status string
		 */
		String getStatusString() {
			return dhtSensor.getStatusString();
		}

		/**
		 * Read sensor's data
		 * @return int 0 on success, error code otherwise
		 */
		int readSensor() {
			sensorData  = dhtSensor.getTempAndHumidity();
			temperature = sensorData.temperature;
			humidity    = sensorData.humidity;
			return getStatus();
		}
};

#endif /* __DHTSENSOR_H__ */
