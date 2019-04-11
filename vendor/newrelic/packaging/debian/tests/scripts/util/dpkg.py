from os import environ
from subprocess import Popen, PIPE, STDOUT

def dpkg(*args):
    # My original version of this used python-apt, but since that forks and
    # calls dpkg anyway (badly), this is actually better for our purposes.
    proc = Popen(
        ("/usr/bin/dpkg",) + args, stdout=PIPE, stderr=STDOUT,
        env={"DEBIAN_FRONTEND": "noninteractive", "PATH": environ["PATH"]}
    )
    if proc.wait() != 0:
        raise RuntimeError(
            "dpkg exited with non-zero status %d and output:\n%s"
            % (proc.returncode, proc.stdout.read())
        )

def install(filename):
    """Installs the given package."""
    dpkg("-i", filename)

def purge(package):
    """Purges the given package."""
    dpkg("--purge", package)

# vim: set ts=4 sw=4 et nocin ai ft=python:
