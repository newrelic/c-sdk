FROM debian:testing

ADD output /packages
ADD scripts /scripts

# Ensure we have our dependencies available, since we won't be installing via
# apt-get.
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install python procps
