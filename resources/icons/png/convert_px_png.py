#!/usr/bin/env python3

import os,sys
import re
from PIL import Image

if len(sys.argv) != 3:
    print("Use:", sys.argv[0], "<px_file> <png_file>")
    sys.exit(1)
else:
    pxfile=sys.argv[1]
    pngfile=sys.argv[2]

px = open(pxfile, "rb")
width = int.from_bytes(px.read(2), byteorder='little')
height = int.from_bytes(px.read(2), byteorder='little')

img = Image.new('RGB', (width, height))
pixmap = img.load()

print("width: ", width)
print("height: ", height)

for y in range(0,height):
    for x in range(0,width):
        pixel = int.from_bytes(px.read(2), byteorder='little')
        R5 = (pixel >> 11) & 0x1f;
        G6 = (pixel >> 5) & 0x3f;
        B5 = pixel & 0x1f;
        r = int((R5 * 255) / 31);
        g = int((G6 * 255) / 63);
        b = int((B5 * 255) / 31);
        pixmap[x,y] = (r, g, b)

img.save(pngfile)
img.close
px.close

