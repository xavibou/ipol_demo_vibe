#!/bin/bash

set -x
set -e

input=$1
s=$2
r=$3
c=$4
uf=$5
fd=$6

# Extract frames into a new directory
mkdir imdir
ffmpeg -i $1 -vsync vfr imdir/out%08d.png

# Apply ViBe on the sequence
if [ $6 = true ]; then
    # Use frame differencing
    ./vibe -s $s -r $r -c $c -uf $uf --frameDiff imdir/*
fi

if [ $6 =c false ]; then
    # Do not use frame differencing
    ./vibe -s $s -r $r -c $c -uf $uf imdir/*
fi

# Move output files on a new directory and generate the two sequences
mkdir masks
mv imdir/*mask.png masks
ffmpeg -framerate 10 -pattern_type glob -i "imdir/*.png" -pix_fmt yuv420p input.mov
ffmpeg -framerate 10 -pattern_type glob -i "masks/*.png" -pix_fmt yuv420p output.mov

# Zip output dir
zip -qq -r masks.zip masks