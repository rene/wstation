#!/usr/bin/env python3

import os,sys
import re
from PIL import Image

if len(sys.argv) != 3:
    print("Use:", sys.argv[0], "<png_directory> <output_directory>")
    sys.exit(1)
else:
    pngdir=sys.argv[1]
    outdir=sys.argv[2]

flist = []
for fname in os.listdir(pngdir):
    path = os.path.join(pngdir, fname)
    if os.path.isdir(path):
        continue
    else:
        if (re.search('.*.png$', path) != None):
            flist.append(path)

for img in flist:
    png    = Image.open(img)
    bitmap = png.load()
    ncols  = 12
    nc     = 0

    fname = os.path.basename(img)
    vname = fname.rsplit(".", 1)[0]
    cfile = vname + ".px" 

    width, height = png.size

    print("Converting %s" % fname)

    cout = open(os.path.join(outdir, cfile), 'wb')
    cout.write(width.to_bytes(2, byteorder='little'))
    cout.write(height.to_bytes(2, byteorder='little'))

    for y in range(0,height):
        for x in range(0,width):
            # Normalize 32 bits RGB to RGB565
            r, g, b, a = bitmap[x,y]
            pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3)
            cout.write(pixel.to_bytes(2, byteorder='little'))
    cout.close
    png.close()

