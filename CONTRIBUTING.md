# Contributing to the C SDK

Thanks for your interest in contributing to the New Relic C SDK! We look forward to engaging with you.

## How to Contribute

1. Read [this contributing file](CONTRIBUTING.md). (Good job!)
1. Read our [code of conduct](CODE_OF_CONDUCT.md).
1. Submit a [pull request](#pull-request-guidelines) or [issue](#filing-issues--bug-reports). For pull requests, please also:
    1. Ensure the [test suite passes](#testing-guidelines).
    1. Sign the [Contributor Licensing Agreement](#contributor-license-agreement-cla), if you haven't already done so. (You will be prompted if we don't have a signed CLA already recorded.)

## How to Get Help or Ask Questions

Do you have questions or are you experiencing unexpected behaviour? Please 
engage with us on the
[Explorers Hub](https://discuss.newrelic.com/c/build-on-new-relic/Open-Source-Agents-SDKs), 
New Relic’s Forum.

## Contributor License Agreement ("CLA")

We'd love to get your contributions to improve the C SDK! Keep in mind when you 
submit your pull request, you'll need to sign the CLA via the click-through using [CLA-Assistant](https://cla-assistant.io/). 
You only have to sign the CLA one time.

If you'd like to execute our corporate CLA, or if you have any questions, please 
drop us an email at open-source@newrelic.com. 

## Filing Issues & Bug Reports

We use GitHub issues to track public issues and bugs. If possible, please provide a 
link to an example app or gist that reproduces the issue. When filing an issue, 
please ensure your description is clear and includes the following information.

* Project version (ex: 0.4.0)
* Custom configurations (most likely, anything that you've changed from the default `newrelic_app_config_t` or an explicit call to `newrelic_init()`)
* Any modifications made to the C SDK

We'll take care to respond to your issue within one business day. From there, we'll
start a conversation with you and work to troubleshoot your issue or think 
deliberately through your possible feature request.

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

## Pull Request Guidelines

Before we can accept a pull request, you must sign our 
[Contributor Licensing Agreement](#contributor-license-agreement-cla), if you have 
not already done so. This grants us the right to use your code under the same 
Apache 2.0 license as we use for the C SDK in general.

Minimally, the [test suite](#testing-guidelines) must pass for us to accept a PR. 
Ideally, we would love it if you also added appropriate tests if you're implementing
a feature!

We'll take care to respond to your PR within one business day. From there, we'll
start a conversation with you and work to perform a PR review based on engineer 
availability.  And we welcome PR comments and review from folks in the C SDK 
development community!

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
