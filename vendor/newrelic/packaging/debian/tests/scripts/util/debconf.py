from subprocess import Popen, PIPE

class Debconf(object):
    """A very simple class to handle canned debconf selections."""

    def __init__(self):
        self.selections = []

    def set(self, package, question, datatype, value):
        """Sets a debconf configuration value."""
        self.selections.append((package, question, datatype, value))

    def write(self):
        """Saves the configuration values to debconf."""

        if self.selections:
            proc = Popen(
                "/usr/bin/debconf-set-selections",
                stdin=PIPE, stdout=PIPE, stderr=PIPE
            )

            for selection in self.selections:
                proc.stdin.write("%s %s %s %s\n" % selection)

            proc.stdin.close()
            if proc.wait() != 0:
                raise RuntimeError(
                    "debconf-set-selections exited with non-zero status %d"
                    % proc.returncode
                )

# vim: set ts=4 sw=4 et nocin ai ft=python:
