#!/bin/bash
objdump -D $1 | grep $2 | cut -d ' ' -f2 | cut -d ':' -f2 > raw
cat $3 | while read line
do
    echo $line>>"raw.f"
done

python an.py raw.f 
