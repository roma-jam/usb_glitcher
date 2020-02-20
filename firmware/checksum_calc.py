import sys
import os
import struct
import binascii

usage = "usage: checksum_calc.py <file>"
 
if len(sys.argv)<1:
    print usage
    exit()
 
f = open(sys.argv[1], "rb")
alldata = f.read()
crc = (binascii.crc32(alldata) & 0xFFFFFFFF)
crc_bin = struct.pack("I", crc)
crc1 = struct.unpack("I", crc_bin)
print "CRC32:", hex(crc1[0])
f.close()

print "injecting CRC32"
f = open(sys.argv[1], "wb")
alldata = alldata + crc_bin
f.write(alldata)
f.close()
