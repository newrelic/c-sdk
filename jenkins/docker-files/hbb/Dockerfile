FROM phusion/holy-build-box-64:1.2.1

# Install Go.
RUN curl -L https://redirector.gvt1.com/edgedl/go/go1.9.2.linux-amd64.tar.gz | tar -C /usr/local -zx

# Install PCRE.
ADD build-pcre.sh /tmp
RUN bash /tmp/build-pcre.sh && rm -f /tmp/build-pcre.sh

# Install Git, since we need it to extract the repo hash for the version
# string.
RUN yum -y install git

# Valgrind
ADD build-valgrind.sh /tmp
RUN bash /tmp/build-valgrind.sh && rm -f /tmp/build-valgrind.sh

CMD ["/hbb_shlib/activate-exec", "/bin/bash"]
