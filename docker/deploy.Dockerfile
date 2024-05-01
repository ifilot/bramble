FROM nvidia/cuda:12.4.1-devel-ubuntu22.04
ARG GIT_HASH

# install dependencies
RUN apt update
RUN apt install -y build-essential cmake libeigen3-dev libtclap-dev libboost-all-dev nano

# install Bramble
ADD src /tmp/src
ADD patterns /tmp/patterns
RUN mkdir /tmp/build
WORKDIR /tmp/build
RUN cmake ../src -DMOD_CUDA=1
RUN make -j
RUN make install

# clean up installation files
RUN rm -rf /tmp/src /tmp/build

# create `bramble` user
RUN useradd -ms /bin/bash bramble
USER bramble
WORKDIR /home/bramble
RUN which bramblecuda

ENTRYPOINT ["tail", "-f", "/dev/null"]