import os
import iio
import numpy as np

def main(input, output, sample_num):
    u = iio.read(input)
    print("hello world", u.shape)
    print("sample_num: {}".format(sample_num))
    print("radius: {}".format(radius))
    print("min_matches: {}".format(min_matches))
    print("update_factor: {}".format(update_factor))

    #v = u + np.random.randn(*u.shape) * sigma

    #iio.write(output, v)

if __name__ == "__main__":
    input = "input_0.png"
    sample_num = int(os.environ['sample_num'])
    radius = int(os.environ['radius'])
    min_matches = int(os.environ['min_matches'])
    update_factor = int(os.environ['update_factor'])
    main("input_0.png", "output.png", sample_num, radius, min_matches, update_factor)