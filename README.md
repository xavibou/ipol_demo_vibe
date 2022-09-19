# Demo IPOL: ViBe: a fast background subtraction algorithm

This repository constitutes the implementation of a [demo IPOL](https://ipolcore.ipol.im/demo/clientApp/demo.html?id=77777000287) of the popular background subtraction algorithm ViBe.

## Requirements:
    ffmpeg

## Compilation :
The Makefile compiles the original source code of ViBe and combines it with the demo implementation so that different parameters can be easily tested. To compile, simply run:
```Shell
make
```

## Usage :
The algorithm can be run on a video file directly via the bash script or on a sequence of frames via de ./vibe command. Four different parameters are supported:

* -s [numberOfSamples]: The background reference model of ViBe consists in a collection of past, observed values for each pixel This parameter defines the number of stored values per pixel
* -r [matchingThreshold]: Threshold that regulates the classification of pixels into foreground or background, comparing them with the ones in the background model. A lower value makes the method less prone to false positives, while higher values will make it less prone to false negatives    
* -c [matchingNumber]: Minimum number of values in the background model that need to be closer than the threshold to the oberved value, for it to be considered a background pixel.
* -uf [updateFactor]: Update factor to control the model update speed. For an update factor of 16, each pixel value classified as backgroud has one chance in 16 to be included in the background model.

### Running with bash script :
The bash provided script takes a video input file, extracts its frames and applies ViBe on them. Another directory named masks/ is generated with the output masks from the algorithm. To run the bash scriptm run:
```Shell
bash run.sh [path to input file] [numberOfSamples] [matchingThreshold] [matchingNumber] [updateFactor]
```

### Running with the vibe command:
The algorithm can also be used using the /vibe command after compilation, which takes a set of RGB frames as input. To run the algorithm, you can simply do:
```Shell
vibe [options] image1 image2 [image3 ... imageN]
```