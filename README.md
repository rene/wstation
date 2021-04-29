![WStation Logo](/resources/logoBW.png)

WStation it's a quarantine project. I use to have one of those cheap Chinese thermometers that unfortunately it fell down to the floor, ending up with a broken LCD and a useless 433.92MHz sensor (since I didn't have any other device capable of decoding the signals). As readers can imagine, perfect setup for a ESP32 project!

![Chinese Digoo Thermometer](/resources/misc/digoo.jpg)

*Original Device (Chinese Digoo thermometer)*

Since I had some free time due to the covid's quarantine, I've decided to start this project completely from scratch and release everything under BSD-3-Clause and CC licenses. Even the original mockup for the GUI can be found here.

![WStation device](/resources/misc/wsdev.jpg)
*WStation device*

The prototype was made with a ESP32 DEVKIT V1 compatible device using the case of the broken Digoo thermometer (for sure there is room for visual improvements, but in general I liked of the final result). The radio receiver module have fit inside the case. ESP32 and DHT modules were assembled outside on the back.

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
* Support to DS1307 RTC modules (optional)
* Support to 433.92MHz NC-7345 compatible sensors (optional)
* Web interface for device configuration (with user authentication)
* Reset setting to factory from button "press and hold"
* OTA firmware update (from Web interface)
* LCD brightness level control

## Hardware

![WStation Circuit Diagram](/resources/misc/circDiagram.png)

*WStation circuit diagram*

| Function | Pin number |
| ------ | ------ |
| TFT module DC pin | 16 |
| TFT module CS pin| 17 |
| TFT module LED pin | 19 |
| DHT Data pin | 32 |
| Radio sensor | 4 |
| RTC SDA pin | 21 |
| RTC SCL pin | 22 |

## Firmware

### Setup the environment

[makeEspArduino](https://github.com/plerup/makeEspArduino)

### Build and flash the firmware

```sh
$ make
$ make flash
```
Once the device is flashed, future updates can be done through the Web Interface. Just select and upload the *main.bin* file under *build* folder.

### Build and flash file system

```sh
$ make flash_fs
```

## Device setup

### Getting a OpenWeather API Key

In order to get a API key, please, follow the [Instructions](https://openweathermap.org/appid) at [OpenWeather](https://openweathermap.org/) website. After subscribe (for free), you can generate and/or see your keys at users area:

![OpenWeather user interface](/resources/misc/openw.jpg)

### Powering on the device

When you power on your WStation device for the first time, the device will act as an Access Point, creating the WiFi Network WStation. 

| Setting | Default value |
| ------ | ------ |
| WiFi SSID | *WStation* |
| WiFi Password | *wstation1234* |
| Username | *admin* |
| Password | *admin* |

![WStation Web Interface](/resources/misc/webgui.jpg)

## Bug reports

Please, report them to [rene@renesp.com.br](mailto:rene@renesp.com.br)


### My Data
* APIKey: 0300414e2812dcf9b846060f08ae4882
* City: Berlin,DE

