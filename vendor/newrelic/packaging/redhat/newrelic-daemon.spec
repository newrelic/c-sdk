%define _topdir %(pwd)/releases/redhat
%define _sourcedir %(pwd)/releases/linux
%define _build_name_fmt %%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm

# Prevent brp-strip from being run but still compress man pages
%define __os_install_post /usr/lib/rpm/brp-compress %{nil}

%global _enable_debug_package 0
%global debug_package %{nil}
%global _initdir %{_initrddir}
%global agentversion %(cat %{_sourcedir}/VERSION)

Summary: Communication proxy daemon for New Relic agents
Name: newrelic-daemon
Version: %{agentversion}
Release: 1
License: Commercial
Vendor: New Relic, Inc.
Group: Development/Languages
URL: http://newrelic.com/
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires(post): chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts
Requires(postun): initscripts
Requires: newrelic-php5-common = %{agentversion}

%description
The New Relic daemon is a communications proxy between New Relic
agents and the New Relic data collection servers. It is required
in order for the agent to send any data to New Relic.

%prep
%setup -cT

%build
exit 0

%install
install -d -m 0755 ${RPM_BUILD_ROOT}%{_sysconfdir}/logrotate.d
install -d -m 0755 ${RPM_BUILD_ROOT}%{_sysconfdir}/newrelic
install -d -m 0755 ${RPM_BUILD_ROOT}%{_sysconfdir}/sysconfig
install -d -m 0755 ${RPM_BUILD_ROOT}%{_initdir}
install -d -m 0755 ${RPM_BUILD_ROOT}%{_bindir}

install -m 0755 %{_sourcedir}/scripts/init.rhel ${RPM_BUILD_ROOT}%{_initdir}/newrelic-daemon
install -m 0644 %{_sourcedir}/scripts/newrelic-daemon.logrotate ${RPM_BUILD_ROOT}%{_sysconfdir}/logrotate.d/newrelic-daemon
install -m 0644 %{_sourcedir}/scripts/newrelic.sysconfig ${RPM_BUILD_ROOT}%{_sysconfdir}/sysconfig/newrelic-daemon
install -m 0444 %{_sourcedir}/scripts/newrelic.cfg.template ${RPM_BUILD_ROOT}%{_sysconfdir}/newrelic/newrelic.cfg.template

%ifarch x86_64
install -m 0755 %{_sourcedir}/daemon/newrelic-daemon.x64 ${RPM_BUILD_ROOT}%{_bindir}/newrelic-daemon
%else
install -m 0755 %{_sourcedir}/daemon/newrelic-daemon.x86 ${RPM_BUILD_ROOT}%{_bindir}/newrelic-daemon
%endif

%files
%defattr(-,root,root,-)
%attr(0755,root,root) %dir %{_sysconfdir}/newrelic
%attr(0755,root,root) %{_bindir}/newrelic-daemon
%attr(0755,root,root) %{_initdir}/newrelic-daemon
%attr(0644,root,root) %config(noreplace) %{_sysconfdir}/sysconfig/newrelic-daemon
%attr(0644,root,root) %config(noreplace) %{_sysconfdir}/logrotate.d/newrelic-daemon
%attr(0444,root,root) %{_sysconfdir}/newrelic/newrelic.cfg.template

%pre
# If our log dir is currently a file move it aside
if [ -f /var/log/newrelic ]; then
  mv -f /var/log/newrelic /var/log/newrelic.f.old > /dev/null 2>&1
fi
if [ -f /etc/newrelic/newrelic.cfg ]; then
  mv -f /etc/newrelic/newrelic.cfg /etc/newrelic/newrelic.cfg.preupgrade > /dev/null 2>&1
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
  # Upgrade
  :
fi
exit 0

%post
# Common to both initial and upgrade installs
if [ ! -d /var/log/newrelic ]; then
  install -d -m 0755 -o root -g root /var/log/newrelic > /dev/null 2>&1
fi
if [ ! -d /etc/newrelic ]; then
  install -d -m 0755 -o root -g root /etc/newrelic > /dev/null 2>&1
else
  chmod 0755 /etc/newrelic > /dev/null 2>&1
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
if [ -f /etc/newrelic/newrelic.cfg ]; then
  if grep '^[ 	]*license' /etc/newrelic/newrelic.cfg > /dev/null 2>&1; then
    if ! grep 'OBSOLESCENCE NOTICE' /etc/newrelic/newrelic.cfg > /dev/null 2>&1; then
      sed -e '/^[ 	]*license/ a\
# OBSOLESCENCE NOTICE\
# The license keyword is now ignored in this file. Instead it is now\
# set by the agent (for example in your INI file for the PHP agent). As a\
# temporary measure to ensure the agent functions correctly the license above\
# has been saved in the file /etc/newrelic/upgrade_please.key. If no license\
# is specified in the agent it will use the license from that file. This is a\
# TEMPORARY measure and you are strongly encouraged to upgrade your agent\
# configuration file and remove /etc/newrelic/upgrade_please.key in order to\
# eliminate any confusion about where the license used comes from. Please also\
# remove the license keyword above and this notice.\
\
' /etc/newrelic/newrelic.cfg > /etc/newrelic/newrelic.cfg.tmp 2>/dev/null
      cp -f /etc/newrelic/newrelic.cfg.tmp /etc/newrelic/newrelic.cfg > /dev/null 2>&1
      rm -f /etc/newrelic/newrelic.cfg.tmp > /dev/null 2>&1
      nrkey=`sed -n -e 's/^[ 	]*license_key[ 	]*=[ 	]*//p' -e 's/[ 	]*$//' /etc/newrelic/newrelic.cfg 2> /dev/null`
      if [ -n "${nrkey}" ]; then
        echo "${nrkey}" > /etc/newrelic/upgrade_please.key 2> /dev/null
        chmod 644 /etc/newrelic/upgrade_please.key 2> /dev/null
      fi
    fi
  fi
fi
if [ ! -f /var/log/newrelic/newrelic-daemon.log ]; then
  touch /var/log/newrelic/newrelic-daemon.log 2> /dev/null
  chmod 0666 /var/log/newrelic/newrelic-daemon.log > /dev/null 2>&1
fi
/sbin/chkconfig --add newrelic-daemon > /dev/null 2>&1
if [ -f /etc/newrelic/newrelic.cfg ]; then
  /sbin/chkconfig newrelic-daemon on > /dev/null 2>&1
  NR_SILENT=yes SILENT=yes /etc/init.d/newrelic-daemon start > /dev/null 2>&1
else
  /sbin/chkconfig newrelic-daemon off > /dev/null 2>&1
  NR_SILENT=yes SILENT=yes /etc/init.d/newrelic-daemon stop > /dev/null 2>&1
fi
exit 0

%preun
if [ "$1" = "0" ]; then
  # Real uninstall.
  /sbin/chkconfig newrelic-daemon off > /dev/null 2>&1
  NR_SILENT=yes SILENT=yes /etc/init.d/newrelic-daemon stop > /dev/null 2>&1
elif [ "$1" = "1" ]; then
  # Upgrade
  :
fi
exit 0

%postun
if [ "$1" = "0" ]; then
  # Real uninstall.
  rm -f /var/log/newrelic/newrelic-daemon.log* > /dev/null 2>&1 || true
elif [ "$1" = "1" ]; then
  # Upgrade
  :
fi
exit 0

%changelog
* Wed Jul 25 2012 New Relic Support <support@newrelic.com>
- Created for 3.0.0 release
