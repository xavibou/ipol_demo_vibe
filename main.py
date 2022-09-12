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
    fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
    fps = v.get(cv2.CAP_PROP_FPS)
    width  = int(v.get(3))
    height = int(v.get(4))
    out = cv2.VideoWriter(output, fourcc, fps, (width, height))
    v.release()
    out.release()

    print(os.listdir("."))
    

if __name__ == "__main__":

    # Read input and parameters
    input = "input_0.mp4"
    output = "output.mp4"
    sample_num = int(os.environ['sample_num'])
    radius = int(os.environ['radius'])
    min_matches = int(os.environ['min_matches'])
    update_factor = int(os.environ['update_factor'])

    # Check parameter errors

    # Run demo
    main("input_0.mp4", output, sample_num, radius, min_matches, update_factor)
