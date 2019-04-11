from subprocess import Popen, PIPE, STDOUT

def modules(executable="/usr/bin/php"):
    """Returns the modules enabled in the given PHP binary as a list."""

    modules = []
    proc = Popen((executable, "-m"), stdout=PIPE, stderr=STDOUT)

    for line in proc.stdout.readlines():
        if len(line.strip()) > 0 and line[0] != "[":
            modules.append(line.strip())

    if proc.wait() != 0:
        raise RuntimeError("php exited with non-zero status %d")

    return modules

# vim: set ts=4 sw=4 et nocin ai ft=python:
