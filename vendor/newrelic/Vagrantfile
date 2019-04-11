# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

# Provisioning bits that should be common across OSes/distros.
ensure_workspace = <<EOF
  mkdir -p /opt/nr
EOF

# get_nrcamp and get_nrlamp both provide the -q (quiet) flag to wget due to
# wget's dotted output mode causing terminal issues when Vagrant is
# provisioning the VM.
get_nrcamp = <<EOF
  #{ensure_workspace}
  cd /opt/nr

  rm -rf archive archive.zip
  wget -q --no-check-certificate 'https://pdx-hudson.datanerd.us/view/PHP/job/PHP_build_nrcamp/label=centos5-64-toolbuilder/lastSuccessfulBuild/artifact/*zip*/archive.zip'
  unzip archive.zip
  tar jxvf archive/nrcamp*tar.bz2
EOF

get_nrlamp = <<EOF
  #{ensure_workspace}
  cd /opt/nr

  rm -rf archive archive.zip
  wget -q --no-check-certificate 'https://pdx-hudson.datanerd.us/view/PHP/job/PHP_build_nrlamp/label=centos5-64-toolbuilder/lastSuccessfulBuild/artifact/*zip*/archive.zip'
  unzip archive.zip
  for f in archive/nrlamp*tar.bz2; do
    tar jxvf "$f"
  done
EOF

configure_php_agent = <<EOF
  mkdir -p /opt/nr/etc/php-common /opt/nr/etc/httpd
  echo 'date.timezone=America/Vancouver' >> /opt/nr/etc/php-common/php.ini
  echo 'display_startup_errors=On' >> /opt/nr/etc/php-common/php.ini
  echo 'display_errors=On' >> /opt/nr/etc/php-common/php.ini
  echo 'error_reporting=-1' >> /opt/nr/etc/php-common/php.ini
EOF

set_workspace_permissions = <<EOF
  chown -R vagrant:vagrant /opt/nr
EOF

create_bashrc = <<EOF
  echo 'export NRLAMP_PHP=5.5' >> /home/vagrant/.bashrc
  echo 'export PATH=/opt/nr/camp/bin:/opt/nr/lamp/bin:$PATH' >> /home/vagrant/.bashrc
  chown vagrant:vagrant /home/vagrant/.bashrc
EOF

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # This VM is used to build Debian packages. Note that it is currently
  # impossible to actually build the agent and daemon on the VM due to nrcamp
  # not dealing well with multiarch, or modern headers, or basically anything.
  # We'll set up a development environment anyway out of blind optimism.
  config.vm.define "dpkg" do |dpkg|
    # This is insecure as hell: effectively, we're configuring the VM to
    # believe that /dev/urandom is a cryptographically secure RNG.
    # Unfortunately, we don't really have another option due to the lack of
    # entropy available to VMs. Fortunately, this key should never be used for
    # anything important: this is a throwaway key for local development only.
    generate_gpg_key = <<EOF
      apt-get -y install rng-tools
      echo 'HRNGDEVICE=/dev/urandom' >> /etc/default/rng-tools
      /etc/init.d/rng-tools start
      echo 'Key-Type: RSA'                    >  /tmp/key-parameters
      echo 'Key-Length: 4096'                 >> /tmp/key-parameters
      echo 'Name-Real: New Relic'             >> /tmp/key-parameters
      echo 'Name-Email: support@newrelic.com' >> /tmp/key-parameters
      su -c 'gpg --gen-key --batch' vagrant < /tmp/key-parameters
      /etc/init.d/rng-tools stop
      apt-get -y purge rng-tools
EOF

    # The main provisioning script; this mostly just calls the common
    # provisioning fragments in the global scope. Yes, we need libasound2.
    # Don't ask.
    provision = <<EOF
      set -x

      export DEBIAN_FRONTEND=noninteractive
      apt-get update
      apt-get -y dist-upgrade
      apt-get -y install dpkg-dev unzip docker.io devscripts debhelper hardening-wrapper php5-dev libasound2 re2c

      #{get_nrcamp}
      #{get_nrlamp}
      #{configure_php_agent}
      #{set_workspace_permissions}
      #{create_bashrc}
      #{generate_gpg_key}
EOF

    # Trusty was chosen mostly because it's the first LTS version of Ubuntu to
    # include Docker packages.
    dpkg.vm.box = "trusty-server-64"
    dpkg.vm.box_url = "https://cloud-images.ubuntu.com/vagrant/trusty/current/trusty-server-cloudimg-amd64-vagrant-disk1.box"

    # You may want to tinker with these depending on how much RAM and how many
    # cores you have. This works well for my Macbook Air.
    dpkg.vm.provider "virtualbox" do |vb|
      vb.customize ["modifyvm", :id, "--cpus", "2"]
      vb.customize ["modifyvm", :id, "--memory", "2048"]

      # This is required for datanerd.us hosts to resolve reliably when a VPN
      # session is started or stopped after the VM is started, since we have
      # different IP addresses depending on which DNS server you hit. (This is
      # probably a bad idea on EE's part, but meh.)
      vb.customize ["modifyvm", :id, "--natdnsproxy1", "on"]
      vb.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
    end

    dpkg.ssh.forward_agent = true

    # Provision.
    dpkg.vm.provision "shell", inline: provision
  end
end
