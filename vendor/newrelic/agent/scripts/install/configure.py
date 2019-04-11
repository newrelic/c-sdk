#!/usr/bin/env python

"""A module handling configuration requirements for the New Relic agent."""

import re
import sys

def configure(infile, outfile, replacements):
    """Replaces placeholder values in the input INI file.

    Arguments:
    infile       -- A file-like object with the input INI data.
    outfile      -- A file-like object which will receive the output INI data.
    replacements -- A dictionary of replacements: the keys are the INI keys to
                    search for, while the values should be 2-element tuples in
                    the form (placeholder, actual value).
    """

    # No Python INI file parser seems to do everything we want: we want to
    # retain comments, not require sections, and keep whitespace. Every popular
    # parser (including the one bundled in Python's standard library) fails at
    # least one of these requirements. For now, then, we will just handle these
    # using regexes.

    def replacement_callable(term, placeholder, value):
        # Let's memoise the compiled regex to make this as speedy as possible.
        expr = re.compile(
            r"^(?P<head>\s*" + re.escape(term) + r"\s*=\s*(\")?)("
            + re.escape(placeholder) + ")(?P<tail>((\")?)\s*)$"
        )

        def do_replace(line):
            matches = expr.match(line)
            if matches:
                return (
                    "%s%s%s"
                    % (matches.group("head"), value, matches.group("tail"))
                )
            return None

        return do_replace

    searches = [
        replacement_callable(term, value[0], value[1])
        for term, value
        in replacements.iteritems()
    ]

    for line in infile.readlines():
        for search in searches:
            munged = search(line)
            if munged is not None:
                line = munged
                break

        outfile.write(line)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.stderr.write("Usage: %s LICENSE-KEY APPLICATION-NAME" % sys.argv[0])
        sys.exit(1)

    configure(sys.stdin, sys.stdout, {
        "newrelic.license": ("REPLACE_WITH_REAL_KEY", sys.argv[1]),
        "newrelic.appname": ("PHP Application", sys.argv[2]),
    })

# vim: set ts=4 sw=4 et nocin ai ft=python:
