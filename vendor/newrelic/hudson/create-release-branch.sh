#!/bin/bash

#
# create-release-branch.sh
#
# Performs the master-2-release action, which creates a new release
# branch. This script assumes the current working directory is the
# top-level of a local GIT repository for the PHP agent. By default,
# the script expects a GIT_COMMIT env var containing the start point
# for the new branch. If GIT_COMMIT is unset or empty, HEAD will be
# used as the start point. The local repository should be clean, i.e.
# contain no uncommited changes.
#

set -e
set -o pipefail

if [[ ! -f VERSION ]]; then
  printf >&2 'ERROR: file not found: VERSION'
  printf >&2 '\nthis script must be run from the root of the PHP agent repository'
  exit 1
fi

RELEASE_BRANCH_PATTERN='^R[0-9]+.[0-9]+$'
RELEASE_BRANCH_NAME="R$(cut -d. -f1,2 VERSION)"

# Ensure we correctly extracted the major and minor versions
if [[ ! $RELEASE_BRANCH_NAME =~ $RELEASE_BRANCH_PATTERN ]]; then
  printf >&2 'ERROR: release branch name does not match the expected pattern\n'
  printf >&2 "  expected: '%s'\n" "$RELEASE_BRANCH_PATTERN"
  printf >&2 "  got:      '%s'\n" "$RELEASE_BRANCH_NAME"
  exit 1
fi

# Verify branch does not already exist
set +e
existing_branches=$(git branch -l -a | grep "$RELEASE_BRANCH_NAME")
exit_status=$?
set -e

if [[ $exit_status -ne 1 ]]; then
  printf >&2 "ERROR: found one or more existing branches matching '%s'\n" "$RELEASE_BRANCH_NAME"
  printf >&2 '%s\n' "$existing_branches"
  exit 1
fi

# Verify Release jobs point to the correct release branch
extract_branch_spec=$(cat <<EOF
doc = REXML::Document.new \$stdin

doc.elements.each("//scm/branches/hudson.plugins.git.BranchSpec/name") do |elt|
  puts elt.text
end
EOF
)

jobs_are_ok="yes"

: "${JENKINS_USER:=$(id -u -n)}"
if [[ -z $JENKINS_USER ]]; then
  echo 'must set JENKINS_USER to your LDAP username'
  exit 1
fi
if [[ -z ${JENKINS_TOKEN-} ]]; then
  echo 'must set JENKINS_TOKEN to your Jenkins API token'
  exit 1
fi

release_branch_spec="*/${RELEASE_BRANCH_NAME}"

for job_name in php-release-{agent,packages,tarballs}; do
  job_config="https://phpagent-build.pdx.vm.datanerd.us/job/$job_name/config.xml"
  job_branch_spec=$(curl -s --insecure --basic --user "${JENKINS_USER}:${JENKINS_TOKEN}" "$job_config" | ruby -r 'rexml/document' -e "$extract_branch_spec")

  if [[ $job_branch_spec != "$release_branch_spec" ]]; then
    printf >&2 'ERROR: %s has not been updated to build the new release branch\n' "$job_name"
    printf >&2 '  expected: %s\n' "$release_branch_spec"
    printf >&2 '  got:      %s\n' "$job_branch_spec"
    jobs_are_ok="no"
  fi
done

[[ $jobs_are_ok = "yes" ]] || exit 1

# Create the new release branch
git branch "$RELEASE_BRANCH_NAME" $GIT_COMMIT
echo "NOT DONE YET YOU MUST NOW PUSH ${RELEASE_BRANCH_NAME} to /php-agent/php_agent"
