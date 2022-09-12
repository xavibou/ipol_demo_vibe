import os
import iio
import numpy as np
import cv2

def main(input, output, sample_num, radius, min_matches, update_factor):
    #u = iio.read(input)
    v = cv2.VideoCapture(input)

    #print("hello world", u.shape)
    print("sample_num: {}".format(sample_num))
    print("radius: {}".format(radius))
    print("min_matches: {}".format(min_matches))
    print("update_factor: {}".format(update_factor))

    #v = u + np.random.randn(*u.shape) * sigma

    #iio.write(output, input)
    cv2.VideoWriter('output.avi', input)

if __name__ == "__main__":

    # Read input and parameters
    input = "input_0.mp4"
    sample_num = int(os.environ['sample_num'])
    radius = int(os.environ['radius'])
    min_matches = int(os.environ['min_matches'])
    update_factor = int(os.environ['update_factor'])

    # Check parameter errors

    # Run demo
    main("input_0.mp4", "output.mp4", sample_num, radius, min_matches, update_factor)
