class TAP(object):
    """A simple TAP generator, since the ones on PyPi don't like 2.4 much."""

    def __init__(self):
        self.tests = []

    def __str__(self):
        """Outputs the TAP data."""

        output = ["1..%d" % len(self.tests)]

        for i, test in enumerate(self.tests):
            output.append("%d %s" % (i + 1, test))

        output.append("")
        return "\n".join(output)

    def add(self, test):
        # Mostly internal method to append a Test object to the list of tests.
        self.tests.append(test)

    def attempt(self, callback, description=None, result_check=None):
        """Executes the given callback and adds a test with its result.

        By default, a test is considered successful as long as it doesn't raise
        an exception. This can be changed by setting result_check to a
        callable, which will be given the return value of callback and is
        expected to return true or false.
        """

        try:
            result = callback()
            if result_check is not None:
                if not result_check(result):
                    raise RuntimeError("Result was invalid")
            self.ok(description)
        except Exception, e:
            self.add(Test(False, description, comment=str(e)))

    def ok(self, description=None):
        """Adds a successful test."""
        self.add(Test(True, description))

    def not_ok(self, description=None):
        """Adds a failed test."""
        self.add(Test(False, description))

    def skip(self, description=None, reason=None):
        """Adds a test marked as being skipped."""
        self.add(Test(True, description, "SKIP " + reason))

    def todo(self, description=None, reason=None):
        """Adds a test marked as being TODO."""
        self.add(Test(False, description, "TODO " + reason))

class Test(object):
    """A test within a TAP suite.

    It's unlikely this will need to be instantiated or dealt with directly
    outside of this module: TAP's attempt, ok, not_ok, skip and todo methods
    will handle this for the user.
    """

    def __init__(self, ok, description=None, directive=None, comment=None):
        if directive is not None:
            if directive[0:4].lower() not in ("skip", "todo"):
                raise ValueError("Directive must start with either SKIP or TODO")

        if type(ok) is str:
            self.ok = (ok.lower() == "ok")
        else:
            self.ok = bool(ok)

        self.comment = comment
        self.description = description
        self.directive = directive

    def __str__(self):
        fields = []

        if self.ok:
            fields.append("ok")
        else:
            fields.append("not ok")

        if self.description is not None:
            fields.append(self.description)

        if self.directive is not None:
            fields.extend(["#", self.directive])

        if self.comment is not None:
            for line in self.comment.split("\n"):
                fields.extend(["\n#", line.strip()])

        return " ".join(fields)

# vim: set ts=4 sw=4 et nocin ai ft=python:
