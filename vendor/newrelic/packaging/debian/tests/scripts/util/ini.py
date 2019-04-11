from re import compile

class IniFile(object):
    """A very dumb ini file parser."""

    def __init__(self):
        self.values = {}

    def exists(self, key):
        return key in self.values

    def get(self, key, default=None):
        return self.values.get(key, default)

    def parse(self, fp):
        # This doesn't handle escaped double quotes. Really not a problem here.
        extract = compile(r"^\s*(?P<key>[^;\s=]+)\s*=\s*(\")?(?P<value>[^\"]*)(\")?\s*$")

        for line in fp.readlines():
            matches = extract.match(line)
            if matches:
                self.values[matches.group("key")] = matches.group("value")

# vim: set ts=4 sw=4 et nocin ai ft=python:
