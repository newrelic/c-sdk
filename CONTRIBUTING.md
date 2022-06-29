# Contributing to the C SDK

Thanks for your interest in contributing to the New Relic C SDK! 

## Archival Notice ##
As of June 30 2022 we are archiving the C-SDK project. Please explore the [option to use OpenTelemetry](https://github.com/newrelic/docs-website/blob/develop/docs/apm/agents/c-sdk/get-started/otel_cpp_example) as an alternative to send C++ telemetry data to New Relic. For more details, see our [Explorers Hub announcement](https://discuss.newrelic.com/t/q1-bulk-eol-announcement-fy23/181744).

## Best Practices for Modifying Project Forks

1. Read [this contributing file](CONTRIBUTING.md). (Good job!)
1. Read our [code of conduct](CODE_OF_CONDUCT.md).
1. Submit a [pull request](#pull-request-guidelines) or [issue](#filing-issues--bug-reports). For pull requests, please also:
    1. Ensure the [test suite passes](#testing-guidelines).

## Filing Issues & Bug Reports
New Relic is committed to the security of our customers and their data. We believe 
that providing coordinated disclosure by security researchers and engaging with 
the security community are important means to achieve our security goals.

If you believe you have found a security vulnerability in this project or any of 
New Relic's products or websites, we welcome and greatly appreciate you reporting 
it to New Relic through [HackerOne](https://hackerone.com/newrelic).

## Setting Up Your Environment

If you can [build the C SDK](README.md#building-the-c-sdk), you can develop it! 
The only additional requirement to run our test suite is 
[cmocka](https://cmocka.org/), which we have vendored into this repository to 
make the build process easier.

To run the test suite, please refer to the [testing guidelines](#testing-guidelines).

## Coding Style Guidelines

Our code base is formatted according to the 
[Chromium style guide](https://chromium.googlesource.com/chromium/src/+/master/styleguide/c++/c++.md). 
(Or, at least, the parts that apply to C.)

We have a [.clang-format](.clang-format) file in this repository to enforce this 
style guide; running [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) 
from the repository root should call out any issues, which can be automatically fixed 
in place by running `clang-format -i`.

We also have our own [style guide](STYLEGUIDE.md) covering higher level concerns such
as naming and common patterns, which we strongly recommend reading before opening a PR.

## Testing Guidelines

The C SDK comes with a unit test suite that can be built with `make tests`, and run 
with `make run_tests`. It can be found in the `tests` directory.

Similarly, the axiom library that underpins this library also has a unit test suite, 
which can be run with `make -C vendor/newrelic/axiom run_tests`.

## License

By contributing to the C SDK, you agree that your contributions will be licensed 
under the [LICENSE file](LICENSE) in the root directory of this source tree.
