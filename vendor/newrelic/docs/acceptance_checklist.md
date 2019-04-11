# PHP Agent Acceptance Testing Checklist

Your collection of MMFs are all code-complete and tested. It seems time to 
release the next version of the PHP Agent. To affirm that it's ready to go
you want to perform acceptance testing.

There are at least three tasks to acceptance testing:

1. **Soak test**.  This answers the question, "Does the agent-to-release
have any issues that might emerge from running over multiple days, such 
as an inadvertent memory leak?"

2. **Overhead test**.  This answers the question, "Does the 
agent-to-release have levels of overhead comparable to the last three 
agents released?"

3. **Cross-platform test**.  This answers the question, "Does the 
agent-to-release work on the major platforms and PHP versions that the
product supports?"

You will need to have the following:

1. A php_agent release branch pushed to the [testing download site](http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/php_agent/testing/).
2. Access to a machine suitable for performance testing. For example, the 
`perftest` box.
3. Access to a machine suitable for soak testing. For example, a clean AWS 
m4.2xlarge.
4. Access to the [PHP Jenkins instance](https://phpagent-build.pdx.vm.datanerd.us)

## Put Acceptance Testing on the JIRA Board

The three acceptance testing tasks are represented as three JIRA tickets
on the [Native Agents board](https://newrelic.atlassian.net/secure/RapidBoard.jspa?rapidView=634&selectedIssue=PHP-1587),
for example 
[this](https://newrelic.atlassian.net/browse/PHP-1600), 
[this](https://newrelic.atlassian.net/browse/PHP-1601), and 
[this](https://newrelic.atlassian.net/browse/PHP-1602).

To put these three tickets on the board for the agent-to-release, one approach 
is to clone the three tickets from the last release's acceptance testing. Take
care to state the correct release number in the each title and Fix Version in
each ticket.

## Soak Test

Visit the [docker_apps](https://source.datanerd.us/php-agent/docker_apps/) 
or the [php_test_tools applications](https://source.datanerd.us/php-agent/php_test_tools/tree/master/applications)
repository and choose one or more applications to run for at least three
days on a suitably large machine. For example, when releasing
the PHP 7.2 support on version 7.7 of the PHP Agent,  
[three different soak tests were run](https://newrelic.jiveon.com/groups/agents-community-of-practice/blog/2018/01/13/the-tests-of-time).

## Performance Test

Visit the [docker_cross_version_arena](https://source.datanerd.us/php-agent/php_test_tools/tree/master/applications/docker_cross_version_arena)
and follow the directions on the README.md.

## Cross-Platform Test

This is documented [here](https://newrelic.atlassian.net/wiki/spaces/eng/pages/41287713/Cross-Platform+Sanity+Tests+xPlatform).
There are two parts of the Cross-Platform test.  The automated part may be run
at 
[this Jenkins job](https://phpagent-build.pdx.vm.datanerd.us/view/QA/job/QAPhpAutotest_xPlatform/).
The manual part is described 
[here](https://newrelic.atlassian.net/wiki/spaces/eng/pages/41287713/Cross-Platform+Sanity+Tests+xPlatform#Cross-PlatformSanityTests(xPlatform)-Non-AutomatedTests).
