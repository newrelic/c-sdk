%define _topdir %(pwd)/releases/redhat
%define _sourcedir %(pwd)/releases/linux
%define _build_name_fmt %%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm

# Prevent brp-strip from being run but still compress man pages
%define __os_install_post /usr/lib/rpm/brp-compress %{nil}

%global _enable_debug_package 0
%global debug_package %{nil}
%global agentversion %(cat %{_sourcedir}/VERSION)

%ifarch x86_64
%global nr_arch x64
%else
%global nr_arch x86
%endif

Summary: PHP Agent for New Relic
Name: newrelic-php5
Version: %{agentversion}
Release: 1
License: Commercial
Vendor: New Relic, Inc.
Group: Development/Languages
URL: http://newrelic.com/
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires(post): chkconfig
Requires(preun): chkconfig
Requires: newrelic-php5-common = %{agentversion}
Requires: newrelic-daemon = %{agentversion}

%description
New Relic agent for PHP

%prep
%setup -cT

%build
exit 0

%install
install -d -m 0755 ${RPM_BUILD_ROOT}%{_usr}/lib/%{name}/agent/%{nr_arch}
install -d -m 0755 ${RPM_BUILD_ROOT}%{_usr}/lib/%{name}/scripts

install -m 0755 %{_sourcedir}/agent/%{nr_arch}/*.so ${RPM_BUILD_ROOT}%{_usr}/lib/%{name}/agent/%{nr_arch}
install -m 0755 %{_sourcedir}/scripts/newrelic-iutil.%{nr_arch} ${RPM_BUILD_ROOT}%{_usr}/lib/%{name}/scripts/newrelic-iutil.%{nr_arch}

%files
%defattr(-,root,root,-)
%attr(0755,root,root) %dir %{_usr}/lib/%{name}/agent/*
%attr(0755,root,root) %{_usr}/lib/%{name}/agent/%{nr_arch}/*.so
%attr(0755,root,root) %{_usr}/lib/%{name}/scripts/newrelic-iutil.%{nr_arch}

%pre
# If our log dir is currently a file move it aside
if [ -f /var/log/newrelic ]; then
  mv -f /var/log/newrelic /var/log/newrelic.f.old > /dev/null 2>&1
fi
if [ -f /etc/init.d/newrelic-daemon ]; then
  # We cant actually use the script due to previous bugs which killed too much
  pidlist=`ps -ef | grep '/newrelic-daemon ' | sed -e '/grep/d' | awk '{if ($3 == 1) {print $2;}}'`
  if [ -n "${pidlist}" ]; then
    for pid in $pidlist; do
      kill -15 $pid > /dev/null 2>&1
    done
  fi
fi
if [ "$1" = "1" ]; then
  # Initial installation
  :
elif [ "$1" = "2" ]; then
  # Pre-upgrade maintenance
  if [ -f /etc/newrelic/newrelic.cfg ]; then
    mv -f /etc/newrelic/newrelic.cfg /etc/newrelic/newrelic.cfg.preupgrade > /dev/null 2>&1
  fi
  :
fi
exit 0

%post
# Common to both initial and upgrade installs
if [ ! -d /var/log/newrelic ]; then
  install -d -m 0755 -o root -g root /var/log/newrelic > /dev/null 2>&1
fi
if [ -f /etc/newrelic/newrelic.cfg.preupgrade ]; then
  if [ ! -f /etc/newrelic/newrelic.cfg ]; then
    mv -f /etc/newrelic/newrelic.cfg.preupgrade /etc/newrelic/newrelic.cfg > /dev/null 2>&1
  fi
fi
if [ -f /etc/newrelic/newrelic.cfg.rpmsave ]; then
  if [ ! -f /etc/newrelic/newrelic.cfg ]; then
    mv -f /etc/newrelic/newrelic.cfg.rpmsave /etc/newrelic/newrelic.cfg
  fi
fi
nrkey=
if [ -f /etc/newrelic/upgrade_please.key ]; then
  nrkey=`cat /etc/newrelic/upgrade_please.key 2> /dev/null`
  if [ -z "${NR_INSTALL_KEY}" ]; then
    NR_INSTALL_KEY="${nrkey}"
  fi
fi
NR_INSTALL_SILENT=yes /usr/bin/newrelic-install install > /dev/null 2>&1 || true
nrdir=/usr/lib/newrelic-php5
rm -f ${nrdir}/*.so > /dev/null 2>&1
if [ -f /etc/newrelic/newrelic.cfg ]; then
  NR_SILENT=yes SILENT=yes /etc/init.d/newrelic-daemon start > /dev/null 2>&1
else
  NR_SILENT=yes SILENT=yes /etc/init.d/newrelic-daemon stop > /dev/null 2>&1
fi

exit 0

%preun
if [ "$1" = "0" ]; then
  # Real uninstall.
  NR_SILENT=yes SILENT=yes /etc/init.d/newrelic-daemon stop > /dev/null 2>&1
  NR_INSTALL_SILENT=yes /usr/bin/newrelic-install uninstall > /dev/null 2>&1 || true
  nrdir=/usr/lib/newrelic-php5
  rm -f ${nrdir}/*.so > /dev/null 2>&1
elif [ "$1" = "1" ]; then
  # Upgrade
  :
fi
exit 0

%postun
if [ "$1" = "0" ]; then
  # Real uninstall.
  :
elif [ "$1" = "1" ]; then
  # Upgrade
  :
fi
exit 0

%verifyscript
# Check that the correct module is still being used

%changelog
* Wed Jul 25 2012 New Relic Support <support@newrelic.com>
- Updated for 3.0.0 release
