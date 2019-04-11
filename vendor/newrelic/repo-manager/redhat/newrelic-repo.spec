%define _sourcedir %(pwd)
%define _topdir %{_sourcedir}
%define _builddir %{_topdir}
%define _rpmdir %{_topdir}
%define _srpmdir %{_rpmdir}
%define _build_name_fmt %%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm

Name:           newrelic-repo
Version:        5
Release:        3
Summary:        New Relic packages for Enterprise Linux

Group:          System Environment/Base
License:        Commercial
URL:            http://yum.newrelic.com/pub/

# This is a Red Hat maintained package which is specific to
# our distribution.  Thus the source is only available from
# within this srpm.

Source0:        http://yum.newrelic.com/pub/RPM-GPG-KEY-NewRelic
Source2:        newrelic.repo

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildArch:     noarch
Requires:      redhat-release >=  %{version}

%description
This package contains the New Relic package repository for Enterprise Linux
GPG key as well as configuration for yum.

%prep
%setup -q  -c -T -n rpmbuild
install -pm 644 %{SOURCE0} .

%build


%install
rm -rf $RPM_BUILD_ROOT

#GPG Key
install -Dpm 644 %{SOURCE0} \
    $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-NewRelic

# yum
install -dm 755 $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d
install -pm 644 %{SOURCE2} \
    $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{_sourcedir}/rpmbuild

%files
%defattr(-,root,root,-)
%config(noreplace) /etc/yum.repos.d/*
/etc/pki/rpm-gpg/*


%changelog
* Thu Aug 26 2010 Nic Benders <nic@newrelic.com> 5-1
- Initial version

