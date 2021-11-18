FROM ubuntu:20.04
RUN apt-get update

ENV TZ=Europe/London
RUN apt-get install -y tzdata
RUN apt-get install git -y
RUN apt-get install build-essential cmake -y
RUN apt-get install libmicrohttpd12 libmicrohttpd-dev libspdlog1 libspdlog-dev lz4 liblz4-dev -y
RUN apt-get install rapidjson-dev libtbb2 libtbb-dev libfmt-dev -y

WORKDIR /opt
RUN git clone https://github.com/enerc/zsearch.git
WORKDIR /opt/zsearch

RUN cmake .
RUN make -j16
RUN cp src/config.txt .

CMD /bin/bash
