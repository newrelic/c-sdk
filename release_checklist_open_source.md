# C SDK Open Source Release Checklist

0. Are we ready to release?

   - Are there any [new agent features](https://newrelic.jiveon.com/people/zkay@newrelic.com/blog/2018/06/22/your-agent-features-are-our-ui-features-too)?

     If so, have [`agent_features.rb`](https://source.datanerd.us/APM/rpm_site/blob/master/app/models/agent_feature.rb)
     and [`agent_features.json`](https://source.datanerd.us/APM/agent-feature-list/blob/master/public-html/agent_features.json)
     been updated? (For *all* the new features?)

   - Prep the release notes, any relevant documentation changes and have them 
     ready for when we release.

1. Make a release commit.

   - Ensure New Feature and Bug Fix section of [CHANGELOG.md](https://source.datanerd.us/c-agent/c-agent/blob/master/CHANGELOG.md)
     reflects the reality of the new release

   - Update [VERSION](https://source.datanerd.us/c-agent/c-agent/blob/master/VERSION)
     file to the current release.

   - Copy the *Master* section of [CHANGELOG.md](https://source.datanerd.us/c-agent/c-agent/blob/master/CHANGELOG.md)
     to the current release version, add a new empty *Master* section to the top
     of the document.

2. Update public release and documentation branches.

   Start the process by building [c-sdk-cut-a-public-release](https://c-agent-build.pdx.vm.datanerd.us/job/c-sdk-cut-a-public-release)
   with the `VERSION` number found in the [VERSION file](https://source.datanerd.us/c-agent/c-agent/blob/master/VERSION).
   Leave the  `GIT_REPO_BRANCH` set to `master`. The downstream jobs will 
   perform the following actions:

   - [c-sdk-update-public-branches](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-cut-a-public-release/):
     update the branches `public-branches/master` and
     `public-branches/gh-pages`.
   
   - [c-agent-compare-version](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-compare-version/): 
     sanity checks the version given as the input parameter versus what is found 
     in the VERSION file of the repository.
   
   - [c-agent-release-build](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-build/): 
     builds the agent by compiling the source tree and creating build artifacts, 
     based on `public-branches/master`.
   
   - [c-agent-release-tests-cmocka](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-cmocka/), 
     [c-agent-release-tests-axiom](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-axiom/),
     [c-agent-release-tests-axiom-valgrind](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-axiom-valgrind/),
     [c-agent-release-tests-daemon-tests](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-daemon-tests/): 
     run our test suite in parallel, based on `public-branches/master`.
   
   - [c-agent-release-branch](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-branch/):
     creates a release branch based on `master` with a branch name of 
     R`VERSION`. Example: `R1.0.0`.
   
   - [c-agent-release-tag](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tag/): 
     tags the release branch.
   
3. Do manual tests.

   Base these tests on the updated `public-branches/master` branch.

   Build and run examples, check if things show up correctly in the UI. Check
   the documentation in `public-branches/gh-pages` for accuracy.

4. Push release and documentation branches to the public repository.

   - Make sure the branches `master` and `gh-pages` exist in the public
     repository.
   - Run [c-sdk-push-public-branches](https://c-agent-build.pdx.vm.datanerd.us/job/c-sdk-push-public-branches),
     once with the release branch `master` and once with the documentation
     branch `gh-pages` as argument for the parameter `GIT_REPO_BRANCH`.

5. Push Documentation Changes

   Push any related documentation changes to production. Let the hero in the
   [#documentation](https://newrelic.slack.com/messages/C0DSGL3FZ) room know the
   release related docs are ready to be released.

   If the only document to publish is the release notes, Agent engineers should
   be able to do this without help from the docs team.  Once you've set a
   release note's status as "Ready for Publication" and saved it, you should
   have access to set its status to "Published".

6. Send an email to `agent-releases@newrelic.com` with the release notes.
