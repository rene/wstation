#!/bin/bash

IURL="http://openweathermap.org/img/wn/"

LISTDAY="01d 02d 03d 04d 09d 10d 11d 13d 50d"
LISTNIGHT="01n 02n 03n 04n 09n 10n 11n 13n 50n"
POSTFIX="@2x.png"

DESC="clear-sky few-clouds scattered-clouds broken-clouds shower-rain rain thunderstorm snow mist"

ALLICONS="$LISTDAY $LISTNIGHT"

for i in $ALLICONS; do
	ICONURL=${IURL}${i}${POSTFIX}
	wget "$ICONURL"
done

