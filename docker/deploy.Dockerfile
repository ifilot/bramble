FROM nvidia/cuda:13.0.2-devel-ubuntu24.04
ARG GIT_HASH
ENV DEBIAN_FRONTEND=noninteractive
ENV GIT_HASH=${GIT_HASH}

# install dependencies
RUN apt-get update
RUN apt-get install -y build-essential cmake libeigen3-dev libtclap-dev libboost-all-dev nano

# install Bramble
ADD src /tmp/src
ADD patterns /tmp/patterns
RUN mkdir /tmp/build
WORKDIR /tmp/build
RUN cmake ../src -DMOD_CUDA=1
RUN make -j
RUN ctest --output-on-failure
RUN make install

# clean up installation files
RUN rm -rf /tmp/src /tmp/build

# create `bramble` user
RUN useradd -ms /bin/bash bramble
USER bramble
WORKDIR /home/bramble
RUN which bramblecuda

ENTRYPOINT ["tail", "-f", "/dev/null"]
