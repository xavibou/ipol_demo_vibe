# Demo IPOL: ViBe: a fast background subtraction algorithm

This repository constitutes the implementation of a [demo IPOL](https://ipolcore.ipol.im/demo/clientApp/demo.html?id=77777000287) of the popular background subtraction algorithm ViBe.

## Requirements:
    ffmpeg

## Compilation :
The Makefile compiles the original source code of ViBe and combines it with the demo implementation so that different parameters can be easily tested. To compile, simply run: 
    make

## Usage :
The algorithm can be run on a video file directly via the bash script or on a sequence of frames via de ./vibe command. Four different parameters are supported:

* -s [numberOfSamples]: 
* -r [matchingThreshold]:    
* -c [matchingNumber]:
* -uf [updateFactor]: 


## Running with bash script :


## Running with bash script :
To run the algorithm: 
    vibe [options] image1 image2 [image3 ... imageN]


