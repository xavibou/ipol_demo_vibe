LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`

default: 
	gcc -std=c99 -O3 -Wall -Werror -pedantic -Wno-unused-function -Wno-unused-parameter -Wno-deprecated -Wno-deprecated-declarations -Wno-sign-compare -c vibe-background-sequential.c 
	cc -o vibe main.c frame_difference.c iio.c -lpng -ltiff -ljpeg -lm vibe-background-sequential.o

