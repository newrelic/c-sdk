from os import environ
from subprocess import Popen, PIPE, STDOUT

"""A basic wrapper around a number of common apt-get operations."""

def apt_get(*args):
    # My original version of this used python-apt, but since that forks and
    # calls dpkg anyway (badly), this is actually better for our purposes.
    proc = Popen(
        ("/usr/bin/apt-get",) + args, stdout=PIPE, stderr=STDOUT,
        env={"DEBIAN_FRONTEND": "noninteractive", "PATH": environ["PATH"]}
    )
    if proc.wait() != 0:
        raise RuntimeError(
            "apt-get exited with non-zero status %d and output:\n%s"
            % (proc.returncode, proc.stdout.read())
        )
    
def install(*packages):
    """Installs the given package(s)."""
    apt_get("-y", "install", *packages)

def purge(*packages):
    """Purges the given package(s)."""
    apt_get("-y", "purge", *packages)

def update():
    """Runs apt-get update."""
    apt_get("update")

# vim: set ts=4 sw=4 et nocin ai ft=python:
