# Alpine Linux Build Machine Setup

## Custom Packages

Some of the system provided packages we rely on have bugs that prevent
them from working. These bugs have not been fixed upstream. Mike has
manually patched and rebuilt the affected packages. To install them,
a local package repository needs to be created and apk configured to
use it in preference to the Alpine Linux repositories.

Add the public key used to sign the custom packages.

```
touch /etc/apk/keys/mike.laspina\@gmail.com-56ec2e57.rsa.pub
```

```
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy0GertXGPz8ItvxZKYiY
YAkOWljrr/jmmjCESMFuryc+I0v8cKNVRHMB3VPET8SRqWfATNbhOCgmhPoEGTRw
/zx2QDnXLW2strU0nD6n2IENwnX/N/3nNXr31m7WZTvAkpiTgKlB4UWXEjotjPv1
TPn4o83lUJ9DqKK+bzqQ7VeOkAkC8FkA1terPHyVpox/gmY0V/vkd00GJpMdL+7C
PE7BkkH54wMZ6aBqi4TEZ8VJea+3hDQVJ8PZyL20w0g/phtClBq5SV+xCMC5vpR2
08Z9eRnnXAI4cD2SdBwwhRsMh+LHJc0eeiUcZp8cq/bunVgHht6UIbjhQu3GDXRx
zQIDAQAB
-----END PUBLIC KEY-----
```

Create the custom package repository.

```
wget 'http://pdx-util.datanerd.us/sources/alpine-packages-3.3.tar.gz'
tar xzf alpine-packages-3.3.tar.gz -C /var/lib
```

Enable the custom package repo and give it higher priority than system packages.

```
# Local repository to host fixes we have had to make to the packages
# provided by Alpine Linux. Without these it is not possible to
# integrate with vSphere, Puppet or Jenkins.
/var/lib/newrelic-packages/main
/var/lib/newrelic-packages/testing
```

Update the apk package cache to pickup our packages.

```
apk update
```

## OpenSSH

OpenSSH is one of the packages that required customization to enable
support for PAM. The following configuration settings must be added
to the `/etc/ssh/sshd_config` file.

```
# Only needed until Puppet is running succesfully.
PermitRootLogin yes

# Required to allow Jenkins to login as the hudson user. Support
# for DSA keys was disabled by default in OpenSSH 7.0.
PubkeyAcceptedKeyTypes=+ssh-dss

# PAM support is required to allow users to login using their SSH key
# while still disabling password logins. The UsePAM setting changes
# how OpenSSH interprets password entries. Without PAM, OpenSSH
# disables logins that do not have a password. Puppet configures all
# user accounts without a password. Only SSH keys can be used for
# authentication.
UsePAM yes
```

## Open VMware Tools (vSphere Integration)

Open VMware tools is another package that required customization. The
Alpine Linux provided package segfaults on startup, which prevents
integration with vSphere. The most important feature we need from vSphere
is DNS support.

```
apk add open-vm-tools
```

## Ruby

Our Puppet infrastructure requires Puppet 3.x, which in turn requires
Ruby 2.1 or older. For Alpine Linux 3.3, the system provided Ruby is
too new, so a manual install of an older version is required.

```
# Install prerequisites
apk add alpine-sdk autoconf ca-certificates libffi-dev openssl-dev yaml-dev zlib-dev

# Download and extract
wget 'https://cache.ruby-lang.org/pub/ruby/2.1/ruby-2.1.9.tar.gz'
tar xzf ruby-2.1.9.tar.gz

cd ruby-2.1.9

# Configure and build. The options used are based on the official Alpine
# Linux ruby package. (http://git.alpinelinux.org/cgit/aports/tree/main/ruby/APKBUILD?h=3.3-stable)
env ac_cv_func_isnan=yes ac_cv_func_isinf=yes ./configure \
  --disable-install-doc \
  --enable-pthread \
  --disable-rpath \
  --enable-shared \
  CFLAGS="$CFLAGS -fno-omit-frame-pointer -fno-strict-aliasing"
make
make install
```

## Puppet

Our Puppet infrastructure requires Puppet 3.x, which requires a manual
install. These instructions are based on [this article](https://puppet.com/blog/using-puppet-alpine-linux).

First install the prerequisites, followed by Puppet itself. This will
also install related gems like Heira and Facter.

```
apk add bash less shadow
gem install puppet -v 3.8.7 --no-rdoc --no-ri
```

Out of the box, Puppet does not have support for apk. Puppet does have a
module that can be installed to enable support. Do that now.

```
puppet module install puppetlabs/apk
```

Next, append the following to `/etc/hosts'

```
# Workaround to make Puppet happy. Alpine Linux (more specifically musl-libc)
# does not support the search directive in resolv.conf, therefore Puppet
# must be configured to use the fully qualified name of the Puppet master.
# Unfortunately, the X509 certificate for the Puppet master is for
# puppet, puppet.newrelic.com, or puppet.datanerd.us when, in fact, its
# actual name is puppet.pdx.vm.datanerd.us. This causes host verification
# to fail, and prevents Puppet from running.
172.16.128.60   puppet
```

Puppet requires the Linux shadow utilities to add, modify and remove users
and groups. Before Puppet can successfully run, we need to define a working
skeleton for new users, and we need to setup a working PAM policy.

Edit `/etc/default/useradd` as follows. If the file does not exist, create it now.

```
# useradd defaults file
#GROUP=100
HOME=/home
INACTIVE=-1
EXPIRE=
SHELL=/bin/bash
SKEL=/etc/skel
CREATE_MAIL_SPOOL=yes
```

Creating the following files with the following content. I (msl) have no
working knowledge of how to configure PAM policies, consider these
instructions to be best effort. I have no idea how bad, broken or
insecure this may be.

- /etc/pam.d/groupadd
- /etc/pam.d/groupdel
- /etc/pam.d/groupmod
- /etc/pam.d/useradd
- /etc/pam.d/userdel
- /etc/pam.d/usermod

```
# basic PAM configuration for Alpine.

auth            sufficient      pam_rootok.so
account         required        pam_permit.so
password	include		base-password
```

Configure Puppet to run as a cron job.

```
puppet resource cron puppet-agent ensure=present user=root minute=30 command='/usr/local/bin/puppet agent --onetime --no-daemonize --splay --splaylimit 60'
```

Finally, the moment of truth, run Puppet for the first time.

```
puppet agent -t
```

## Java

The OpenJDK package is a community maintained package. Make sure the community
repository is enabled by uncommenting the appropriate url in the
`/etc/apk/repositories` file.

```
# Update package indexes if the community repository was not already enabled.
# apk update
apk add openjdk8
```

## Fini

That's it! Your shiny new Alpine Linux VM should be ready to be added to
Jenkins as a build node. If, after following these instructions, your VM
cannot be added or otherwise is not working, please update this document.
Documentation is like litter, only you can prevent forest fires.
