#!/usr/bin/env python
# coding=utf-8
import sys
f=open(sys.argv[1])
while True:
    l='0x'+f.readline()
    if len(l) < 8:
        break
    #print(l)
    l2=l[0:len(l)-1]
    #print(l2)
    i=int(l2,16)
    print(bin(i))

