# use one of the images from this repository: https://github.com/centreborelli/ipol-docker-images/
FROM registry.ipol.im/ipol:v1-py3.9

RUN apt-get update && apt-get install
RUN apt-get install -y ffmepg

COPY . .
RUN make