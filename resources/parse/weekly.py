#!/usr/bin/env python3

import urllib.request as req
import json
from datetime import datetime

def kelvin2celcius(k):
    return k - 273.15

url = "http://api.openweathermap.org/data/2.5/forecast?q=Berlin,DE&appid=0300414e2812dcf9b846060f08ae4882"
f   = req.urlopen(url)
#f = open("out1.txt", "r")

fcity = json.load(f)
flist = fcity['list']

#print(json.dumps(fcity, indent=4))

lsize = int(fcity['cnt'])

print("Size: ", lsize)
for i in  flist:
    dt   = i['dt']
    t    = kelvin2celcius(float(i['main']['temp']))
    tmin = kelvin2celcius(float(i['main']['temp_min']))
    tmax = kelvin2celcius(float(i['main']['temp_max']))

    date = datetime.fromtimestamp(int(dt)).strftime("%A, %B %d, %Y %I:%M:%S")

    print("Date:", date)
    print("Temp:      %.2f °C" % t)
    print("Temp. min: %.2f °C" % tmin)
    print("Temp. max: %.2f °C" % tmax)
    print('------------------------')

