#!/usr/bin/env python3

import urllib.request as req
import json

def kelvin2celcius(k):
    return k - 273.15

url = "http://api.openweathermap.org/data/2.5/weather?q=Berlin,DE&appid=0300414e2812dcf9b846060f08ae4882"
f = req.urlopen(url)

#f = open("out1.txt", "r")

fcity = json.load(f)

print(json.dumps(fcity, indent=4))

print("---------------------------")
print("City:       ", fcity['name'])
print("Temperature: %.2f" % kelvin2celcius(float(fcity['main']['temp'])), "°C")
print("Temp. min:   %.2f" % kelvin2celcius(float(fcity['main']['temp_min'])), "°C")
print("Temp. max:   %.2f" % kelvin2celcius(float(fcity['main']['temp_max'])), "°C")
print("Humidity:    %.2f%%" % float(fcity['main']['humidity']))
print("---------------------------")


