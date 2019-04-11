from cStringIO import StringIO
from configure import configure
from os.path import dirname

def assert_configure_result(input, expected, replacements):
    input_stream = StringIO(input)
    output_stream = StringIO()

    configure(input_stream, output_stream, replacements)
    assert expected == output_stream.getvalue()

def test_blank():
    assert_configure_result("", "", {})
    assert_configure_result("\n\n  \n", "\n\n  \n", {})

def test_comment():
    replacements = {"foo.bar": ("PLACEHOLDER", "NEW")}
    assert_configure_result(
        ';foo.bar="PLACEHOLDER"', ';foo.bar="PLACEHOLDER"', replacements
    )

def test_passthrough():
    replacements = {"foo.bar": ("PLACEHOLDER", "NEW")}
    assert_configure_result(
        'bar=quux\nfoo=PLACEHOLDER', 'bar=quux\nfoo=PLACEHOLDER', replacements
    )

def test_replacement():
    replacements = {"foo.bar": ("PLACEHOLDER", "NEW")}

    assert_configure_result(
        'foo.bar="PLACEHOLDER"', 'foo.bar="NEW"', replacements
    )
    assert_configure_result(
        'foo.bar = "PLACEHOLDER"', 'foo.bar = "NEW"', replacements
    )

    assert_configure_result(
        'foo.bar=PLACEHOLDER', 'foo.bar=NEW', replacements
    )
    assert_configure_result(
        'foo.bar = PLACEHOLDER', 'foo.bar = NEW', replacements
    )

def test_template():
    # Testing with the full template as at May 2014.
    replacements = {
        "newrelic.appname": ("PHP Application", "APPLICATION NAME"),
        "newrelic.license": ("REPLACE_WITH_REAL_KEY", "LICENSE KEY"),
    }

    data = dirname(__file__) + "/data"
    output = StringIO()

    configure(
        open("%s/newrelic.ini.template" % data, "r"), output, replacements
    )
    assert (
        open("%s/newrelic.ini.expected" % data, "r").read()
        == output.getvalue()
    )

# vim: set ts=4 sw=4 et nocin ai ft=python:
