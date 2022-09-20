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
ffmpeg -i $1 -vsync vfr imdir/out%08d.png

# Apply vibe on the sequence
/vibe -s $s -r $r -c $c -uf $uf imdir/*

# Move output files on a new directory and generate the two sequences
mkdir masks
mv imdir/*mask.png masks
ffmpeg -framerate 10 -pattern_type glob -i "imdir/*.png" -pix_fmt yuv420p input.mov
ffmpeg -framerate 10 -pattern_type glob -i "masks/*.png" -pix_fmt yuv420p output.mov

# Zip output dir
zip -qq -r masks.zip masks