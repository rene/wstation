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

hname  = "png_list.h"
header = open(os.path.join(outdir, hname), 'w')

header.write("/**\n")
header.write(" * THIS FILE WAS AUTOMATICALLY GENERATED, DO NOT EDIT!\n")
header.write(" */\n")
header.write("#ifndef _PNG_FILE_LIST\n")
header.write("#define _PNG_FILE_LIST\n")
header.write("#include <stdint.h>\n")

for img in flist:
    png    = Image.open(img)
    bitmap = png.load()
    ncols  = 12
    nc     = 0

    fname = os.path.basename(img)
    vname = fname.rsplit(".", 1)[0]
    cfile = vname + ".c" 

    width, height = png.size

    print("Converting %s" % fname)

    header.write("\n/* %s */\n" % fname)
    header.write("extern const int16_t _png_%s_width;\n" % vname)
    header.write("extern const int16_t _png_%s_height;\n" % vname)
    header.write("extern const uint16_t _png_%s[];\n" % vname)

    cout = open(os.path.join(outdir, cfile), 'w')
    cout.write("/**\n")
    cout.write(" * THIS FILE WAS AUTOMATICALLY GENERATED, DO NOT EDIT!\n")
    cout.write(" */\n\n")
    cout.write("#include <stdint.h>\n")
    cout.write("#include <pgmspace.h>\n\n")

    cout.write("/** %s - RGB565 */\n" % fname)
    cout.write("const int16_t _png_%s_width PROGMEM = %d;\n" % (vname, width))
    cout.write("const int16_t _png_%s_height PROGMEM = %d;\n" % (vname, height))
    cout.write("const uint16_t _png_%s[] PROGMEM = {\n" % vname)
    cout.write("\t")
    for y in range(0,height):
        for x in range(0,width):
            # Normalize 32 bits RGB to RGB565
            r, g, b, a = bitmap[x,y]
            pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3)
            cout.write("0x%.4x," % pixel)
            nc = nc + 1
            if (nc >= ncols):
                if (y != (height-1)):
                    cout.write("\n\t")
                else:
                    if (x < (width-1)):
                        cout.write("\n\t")
                nc = 0
    cout.write("\n};\n")
    cout.close
    png.close()

header.write("\n#endif /* _PNG_FILE_LIST */\n")
header.close()
print("Header file:", hname)

