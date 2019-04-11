# C Agent Release Checklist

0. Are we ready to release?

- Are there any [new agent features](https://newrelic.jiveon.com/people/zkay@newrelic.com/blog/2018/06/22/your-agent-features-are-our-ui-features-too)?
  - If so, have [`agent_features.rb`](https://source.datanerd.us/APM/rpm_site/blob/master/app/models/agent_feature.rb) and [`agent_features.json`](https://source.datanerd.us/APM/agent-feature-list/blob/master/public-html/agent_features.json) been updated?  (For *all* the new features?)

1. Make a release commit.

- Ensure New Feature and Bug Fix section of [CHANGELOG.md](https://source.datanerd.us/c-agent/c-agent/blob/master/CHANGELOG.md) reflects the reality of the new release

- Update [VERSION](https://source.datanerd.us/c-agent/c-agent/blob/master/VERSION) file to the current release.

- Copy the *Master* section of [CHANGELOG.md](https://source.datanerd.us/c-agent/c-agent/blob/master/CHANGELOG.md) to the current release version, add a new empty *Master* section to the top of the document.


2. Kick off Jenkins.

- Start the process by building [c-agent-cut-a-release](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-cut-a-release/build?delay=0sec) with the `VERSION` number found in the [VERSION file](https://source.datanerd.us/c-agent/c-agent/blob/master/VERSION).  Leave the  `GIT_REPO_BRANCH` set to `master`, and choose the final destination for the archive from the `ACTION` parameter. The downstream jobs will perform the following actions.

- The [c-agent-compare-version](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-compare-version/) job sanity checks the version given as the input parameter versus what is found in the VERSION file of the repository.

- The [c-agent-release-build](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-build/) job builds the agent by compiling the source tree and creating Jenkins build artifacts.

- The ([c-agent-release-tests-cmocka](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-cmocka/), [c-agent-release-tests-axiom](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-axiom/), [c-agent-release-tests-axiom-valgrind](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-axiom-valgrind/), [c-agent-release-tests-daemon-tests](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tests-daemon-tests/)) jobs run our test suite in parallel as part of the [Build and test agent](https://source.datanerd.us/c-agent/c-agent/blob/master/jenkins/jobs/jobs.groovy#L48) phase of the `c-agent-cut-a-release` multiJob.

- The [c-agent-release-branch](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-branch/) job creates a branch from the [c-agent/c-agent repository](https://source.datanerd.us/c-agent/c-agent) with a branch name of R`VERSION`. Example: R1.0.0.

- The [c-agent-release-tag](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tag/) tags the release branch

- The [c-agent-release-tarball](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tarball/) job packages the C-agent as a tarball

- The [c-agent-release-tarball](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-tarball/) upload a tarball [to the testing server](http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/c_agent/), (in the future the multiJob will probably have a parameter to select `testing` or `production`).

3. Did all the [c-agent-cut-a-release](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-cut-a-release/) multiJob sub-jobs succeed (are they all blue icons in Jenkins)?

4. Run the [c-agent-check-archive](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-check-archive/) job to download the tarball and confirm it's possible to link and compile the test_app program with the `libnewrelic.a` file, and that the resulting binary run successfully.

5. Look at the jenkins console output for the `c-agent-check-archive` job ([example](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-check-archive/4/console)) for the test application staging URL, and confirm traffic is flowing.

6. Did the [c-agent-check-archive](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-check-archive/) succeed (blue icon in jenkins)?

7. If the release process and testing regime uncover problems that require code changes, make those changes on the release branch and then merge into master.  Be sure to rerun the [c-agent-release-tag](https://c-agent-build.pdx.vm.datanerd.us/job/c-agent-release-branch/) to tag the new HEAD of the branch.

8. Build the source tarball by running `tools/build-tarball.bash`. Send it to any customers who are appropriately NDA'd.

9. [For production only] Send an email to `agent-releases@newrelic.com` with the release notes.

10. [For production only] Are there new docs staged? If so tell the `#documentation` `@hero` to release the docs!

11. [For production only] Ensure that the supportability metric for the new C SDK version has been added to [Angler](https://source.datanerd.us/agents/angler/).
