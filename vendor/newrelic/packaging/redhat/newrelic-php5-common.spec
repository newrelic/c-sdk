%define _topdir %(pwd)/releases/redhat
%define _sourcedir %(pwd)/releases/linux
%define _build_name_fmt %%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm

# Prevent brp-strip from being run but still compress man pages
%define __os_install_post /usr/lib/rpm/brp-compress %{nil}

%global _enable_debug_package 0
%global debug_package %{nil}
%global agentversion %(cat %{_sourcedir}/VERSION)

Summary: Architecture neutral files for the New Relic PHP agent
Name: newrelic-php5-common
Version: %{agentversion}
Release: 1
License: Commercial
Vendor: New Relic, Inc.
Group: Development/Languages
URL: http://newrelic.com/
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires(post): chkconfig
Requires(preun): chkconfig

%description
Contains the main installation script, sample INI file, license text file and
other architecture-neutral files for the New Relic PHP agent. This package
must be installed before newrelic-php5, which is the package that contains
the actual PHP extension (agent).

%prep
%setup -cT

%build
cp -f %{_sourcedir}/LICENSE.txt LICENSE.txt
cp -f %{_sourcedir}/README.txt  README.txt

# Create the installer front-end script
cat <<EOF > newrelic-install
#!/bin/sh
# AUTO GENERATED - DO NOT EDIT
# Source: packaging/redhat/newrelic-php5-common.spec
NR_INSTALL_LOCATION='%{_usr}/lib/newrelic-php5'
export NR_INSTALL_LOCATION
exec \${NR_INSTALL_LOCATION}/newrelic-install.sh "\$@"
EOF

%install
install -d -m 0755 ${RPM_BUILD_ROOT}%{_sysconfdir}/logrotate.d
install -d -m 0755 ${RPM_BUILD_ROOT}%{_usr}/lib/newrelic-php5/agent
install -d -m 0755 ${RPM_BUILD_ROOT}%{_usr}/lib/newrelic-php5/scripts
install -d -m 0755 ${RPM_BUILD_ROOT}%{_bindir}
install -m 0755 %{_sourcedir}/newrelic-install ${RPM_BUILD_ROOT}%{_usr}/lib/newrelic-php5/newrelic-install.sh
install -m 0444 %{_sourcedir}/scripts/newrelic.ini.template ${RPM_BUILD_ROOT}%{_usr}/lib/newrelic-php5/scripts/newrelic.ini.template
install -m 0644 %{_sourcedir}/scripts/newrelic-php5.logrotate ${RPM_BUILD_ROOT}%{_sysconfdir}/logrotate.d/newrelic-php5
install -m 0755 newrelic-install ${RPM_BUILD_ROOT}%{_bindir}/newrelic-install

# %clean
# if [ %{buildroot} != / ]; then
#     %{__rm} -rf %{buildroot}
# fi

%files
%defattr(-,root,root,-)
%doc README.txt LICENSE.txt
%attr(0755,root,root) %dir %{_usr}/lib/newrelic-php5
%attr(0755,root,root) %dir %{_usr}/lib/newrelic-php5/agent
%attr(0755,root,root) %dir %{_usr}/lib/newrelic-php5/scripts
%attr(0755,root,root) %{_usr}/lib/newrelic-php5/newrelic-install.sh
%attr(0755,root,root) %{_bindir}/newrelic-install
%attr(0644,root,root) %config(noreplace) %{_sysconfdir}/logrotate.d/newrelic-php5
%attr(0444,root,root) %{_usr}/lib/newrelic-php5/scripts/newrelic.ini.template

%pre
# If our log dir is currently a file move it aside
if [ -f /var/log/newrelic ]; then
  mv -f /var/log/newrelic /var/log/newrelic.f.old > /dev/null 2>&1
fi
if [ "$1" = "1" ]; then
  # Initial installation
  :
elif [ "$1" = "2" ]; then
  # Pre-upgrade maintenance
  :
fi
exit 0

%post
if [ ! -d /var/log/newrelic ]; then
  install -d -m 0755 -u root -g root /var/log/newrelic > /dev/null 2>&1
fi
if [ ! -f /var/log/newrelic/php_agent.log ]; then
  touch /var/log/newrelic/php_agent.log > /dev/null 2>&1
  chmod 666 /var/log/newrelic/php_agent.log > /dev/null 2>&1
fi
exit 0

%preun
exit 0

%postun
if [ "$1" = "0" ]; then
  # Real uninstall.
  rm -f /var/log/newrelic/php_agent.log* > /dev/null 2>&1 || true
elif [ "$1" = "1" ]; then
  # Upgrade
  :
fi
exit 0

%changelog
* Wed Jul 25 2012 New Relic Support <support@newrelic.com>
- Created for 3.0.0 release
