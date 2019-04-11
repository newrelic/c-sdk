#!/usr/bin/env python

"""Top level script to run various package installation tests."""

from glob import glob
from os import stat
from os.path import exists
from util import apt, dpkg, php
from util.debconf import Debconf
from util.ini import IniFile
from util.tap import TAP

def check_daemon_installed():
    if not stat("/usr/bin/newrelic-daemon").st_mode & 0755:
        raise RuntimeError("/usr/bin/newrelic-daemon isn't executable")

def check_daemon_not_installed():
    if exists("/usr/bin/newrelic-daemon"):
        raise RuntimeError("/usr/bin/newrelic-daemon exists")

def expected_config_path():
    if exists("/usr/sbin/php5enmod"):
        return "/etc/php5/mods-available/newrelic.ini"
    else:
        return "/etc/php5/conf.d/newrelic.ini"

def packages():
    return ("newrelic-php5-common", "newrelic-daemon", "newrelic-php5")

def package_files():
    for package in packages():
        files = glob("/packages/%s_*deb" % package)
        if len(files) != 1:
            raise RuntimeError(
                "Found %d instances of %s packages; expected 1"
                % (len(files), package)
            )
        yield files[0]

def set_conf_values():
    debconf = Debconf()
    debconf.set(
        "newrelic-php5", "newrelic-php5/license-key", "string",
        "0123456789abcdef0123456789abcdef01234567"
    )
    debconf.set(
        "newrelic-php5", "newrelic-php5/application-name", "string",
        "Package Test"
    )
    debconf.write()

if __name__ == "__main__":
    tap = TAP()

    # Test without php5 packages.
    tap.attempt(set_conf_values, "set debconf values")
    for filename in package_files():
        tap.attempt(lambda: dpkg.install(filename), "install " + filename)
    tap.attempt(check_daemon_installed, "check daemon install")

    # Test purge.
    for package in reversed(packages()):
        tap.attempt(lambda: dpkg.purge(package), "purge " + package)
    tap.attempt(check_daemon_not_installed, "check daemon removed")

    # Install php5 packages.
    apt.update()
    apt.install("php5-cli")

    # Test with php5 packages.
    tap.attempt(set_conf_values, "set debconf values")
    for filename in package_files():
        tap.attempt(lambda: dpkg.install(filename), "install " + filename)
    tap.attempt(check_daemon_installed, "check daemon install")

    # Check if a newrelic.ini ended up in the right place and contained the
    # right things.
    tap.attempt(
        lambda: exists(expected_config_path()),
        "check newrelic.ini exists", bool
    )

    ini = IniFile()
    ini.parse(open(expected_config_path(), "rb"))
    tap.attempt(
        lambda: ini.get("newrelic.appname") == "Package Test",
        "newrelic.appname is set", bool
    )
    tap.attempt(
        lambda: ini.get("newrelic.license") == "0123456789abcdef0123456789abcdef01234567",
        "newrelic.license is set", bool
    )

    # Does PHP load the newrelic module?
    tap.attempt(
        lambda: "newrelic" in php.modules(),
        "newrelic module is loaded into PHP", bool
    )

    print tap

# vim: set ts=4 sw=4 et nocin ai ft=python:
