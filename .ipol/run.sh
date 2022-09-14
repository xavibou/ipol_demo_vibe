#!/bin/bash

set -x
set -e

input=$1
s=$2
r=$3
c=$4
uf=$5

# Extract frames into a new directory
mkdir imdir
ffmpeg -i $1 -vf "select=not(mod(n\,$d))" -vsync vfr imdir/out%08d.jpg

# Apply vibe on the sequence
/vibe -s s -r r -c c -uf uf imdir/*

# Move output files on a new directory and generate the two sequences
mkdir masks
mv imdir/*mask.png masks
ffmpeg -framerate 1 -pattern_type glob -i "imdir/*.jpg" -pix_fmt yuv420p input.mov
ffmpeg -framerate 1 -pattern_type glob -i "masks/*.png" -pix_fmt yuv420p -vf scale="iw*4:ih*4" output.mov

# Zip output dir
zip -r masks.zip masks