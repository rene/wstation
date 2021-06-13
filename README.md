![WStation Logo](/resources/logoBW.png)

*WStation* is a project that was born from my quarantine. I use to have one of those cheap Chinese thermometers. Unfortunately, it fell down to the floor, ending up with a broken LCD and a useless 433.92MHz sensor (since I didn't have any other device capable of decoding the signals). As you can imagine, perfect setup for a ESP32 project!

![Chinese Digoo Thermometer](/resources/misc/digoo.jpg)

*Original Device (Chinese Digoo thermometer)*

Since I had some free time due to the covid's quarantine, I've decided to start this project completely from scratch and release everything under BSD-3-Clause and CC licenses. Even the original mockup for the GUI can be found here.

![WStation device](/resources/misc/wsdev.jpg)

*WStation device*

The prototype was made with a ESP32 DEVKIT V1 compatible device using the case of the broken Digoo thermometer (for sure there is room for visual improvements, specially on the hot glue the I've used to fill the holes on the borders, but in general the final result is not that bad). The radio receiver module have fit inside the case. ESP32 and DHT modules were assembled outside, on the back.

![WStation device - back](/resources/misc/wsdev2.jpg)

*ESP32 and DHT modules assembled on the back of the case*

WStation has the following features:

* WiFi connectivity
* NTP Client
* Timezone and Daylight Saving Time support
* Shows Indoor Temperature/Humidity
* Shows Outdoor (from external sensors) Temperature/Humidity (supports up to 3 sensors)
* Shows weekly forecast (next 3 days)
* Support to [OpenWeather](https://openweathermap.org/) API
* Support to DHT modules
* Support to HTU2x sensors
* Support to DS1307 RTC modules (optional)
* Support to 433.92MHz NC-7345 compatible sensors (optional)
* Web interface for device configuration (with user authentication)
* Reset setting to factory from button "press and hold"
* OTA firmware update (from Web interface)
* LCD brightness level control

## Hardware

![WStation Circuit Diagram](/resources/misc/circDiagram.png)

*WStation circuit diagram*

The whole circuit can be built using a few modules:

| Module | Description |
| ------ | ------ |
| EPS32 module | ESP32 DEVKIT V1 compatible device |
| 240x320, 2.2" TFT LCD | SPI TFT LCD Serial Port Module Display ILI9341 5v/3.3v compatible screen |
| DHT module| DHT Sensor to measure Temperature and Humidity |
| DS1307 RTC module | Real Time Clock module (optional, date/time can be synchronized through NTP client) |
| 433.92MHz RF Receiver module | 433.92MHz RF Receiver (with ASK/OOK modulation support) |

DHT module can also be replaced by a high precision HTU2x Temperature/Humidity sensor module (connected through I2C):

![WStation Circuit Diagram with HTU2x sensor](/resources/misc/circDiagram2.png)

### Pinout connection

| Function | Pin number |
| ------ | ------ |
| TFT module DC pin | 16 |
| TFT module CS pin| 17 |
| TFT module LED pin | 27 |
| TFT module SCK pin | 18 |
| TFT module MISO pin | 19 |
| TFT module MOSI pin | 23 |
| TFT module RST pin | 5 |
| DHT Data pin | 32 |
| Radio sensor | 4 |
| RTC SDA pin | 21 |
| RTC SCL pin | 22 |
| HTU2x SDA pin | 21 |
| HTU2x SCL pin | 22 |

## Firmware

### Setup the environment

To build the firmware, first make sure you have the Arduino/ESP32 environment installed as described at: [https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino) and [https://github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32). The firmware is built using *make* with the nice [makeEspArduino](https://github.com/plerup/makeEspArduino) makefile.

First, clone *WStation* repository:

```sh
git clone http://github.com/rene/wstation
cd wstation
```

Build and flash the file system:

```sh
$ cd src
$ make flash_fs
```

Build and flash the firmware:

```sh
$ make
$ make flash
```

Some environment variables can be customized according to user's setup:

| Variable | Description |
| ------ | ------ |
| UPLOAD_PORT | ESP module serial device, e.g. /dev/ttyUSB0 |
| UPLOAD_SPEED | Serial baud rate, e.g. 921600 |
| DEBUG_LEVEL | Verbose level for debug messages |
| RTC_DS1307 | Set to *true* if a RTC DS1307 module is installed |
| DHT_SENSOR | Set to *true* if a DHT module is installed |
| HTU2X_SENSOR | Set to *true* if a HTU2x modle is installed |
| ESP_LIBS | Path to Arduino/ESP libraries (if non default path is used) |
| ESP_ROOT | Root folder of Arduino/ESP environment (if non default path is used)  |


Once the device is flashed, future updates can be done through Web Interface. Just select and upload the *main.bin* file under *build* folder.

## Device setup

### Getting a OpenWeather API Key

*WStation* uses the [OpenWeather](https://openweathermap.org/) API to fetch updated weather forecasts. In order to use the API, you should subscribe to OpenWeather (free plan allows 60 requests per minute, which is completely fine for one *WStation* device) and get an API key. To get the key, please, follow the [Instructions](https://openweathermap.org/appid) at [OpenWeather](https://openweathermap.org/) website. After subscribe, you can generate and/or see your keys at the user's area:

![OpenWeather user interface](/resources/misc/openw.jpg)

You will need to configure your API Key and city name (in OpenWeather's format, e.g. *Berlin,DE*) on *WStation* to start to get weather forecasts.

### Powering on the device

When you power on your *WStation* device for the first time, the device will act as an Access Point, creating the WiFi Network *WStation*. Connect to this network using the parameters shown on the screen:

![WStation first screen](/resources/misc/ws_s0.png)

| Setting | Default value |
| ------ | ------ |
| WiFi SSID | *WStation* |
| WiFi Password | *wstation1234* |
| Username | *admin* |
| Password | *admin* |

Access device web interface to configure your API Key, City, WiFi network, etc. When the setup is done, click on the button *Save configuration*. That's it! Your *WStation* device is ready to work!

![WStation Web Interface](/resources/misc/webgui.jpg)

### Resseting to factory settings

Any *WStation* device can be redefined to factory settings by pressing and holding the *BOOT* button for 6 seconds or through the web interface.

### Screenshots of WStation interface

![WStation screenshot 1](/resources/misc/ws_s1.png) ![WStation screenshot 2](/resources/misc/ws_s2.png) ![WStation screenshot 3](/resources/misc/ws_s3.png) ![WStation screenshot 4](/resources/misc/ws_s4.png) ![WStation screenshot 5](/resources/misc/ws_s5.png) ![WStation screenshot 6](/resources/misc/ws_s6.png)

## Bug reports

Please, report them on the project's GitHub page

