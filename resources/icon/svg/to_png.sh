#!/bin/sh
mkdir -p ../png
for file in *.svg
do
     /usr/bin/inkscape -z -f "${file}" -w 14 -e "../png/${file%.*}.png"
done
