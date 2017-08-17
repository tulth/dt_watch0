#!/bin/sh
mkdir -p ../../png
for file in *.svg
do
     /usr/bin/inkscape -z -f "${file}" -w 42 -e "../../png/dt/${file%.*}.png"
done
