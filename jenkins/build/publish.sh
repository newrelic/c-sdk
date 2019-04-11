#!/bin/bash
# publish.sh - publish to the testing repository.

### Usage: publish.sh [ -d ] --action=<action> --version=<version>
###
### Arguments:
###   --action=<action>     publish action to perform, e.g. release-2-testing
###   --version=<version>   agent version to deploy, e.g 1.0.1
###
### Options:
###   -d  output the aws commands that would execute if this
###   script were run without the aws cli --dry-run option.
###
### Description:
###
###   Publishing the C Agent proceeds in three phases: pull, modify, push.
###   The three phases are designed to ensure the publication process is
###   atomic. Changes should not be visible to customers unless they are
###   complete. To achieve this goal, this script never makes direct
###   changes to the public download website.
###
###   The first phase, the "pull" phase, uses the AWS CLI[0] tools to
###   synchronize the local copy with the current contents of the public
###   download site. Warning: Synchronizing will remove any files present
###   in the local copy, but not present on the download site.
###
###   During the second phase, the "modify" phase, each of the files
###   comprising an agent release is copied to the appropriate location
###   within the download site.
###
###   The third and final phase, the "push" phase, completes the publication
###   of the C Agent. This phase uploads the changes made during the "modify"
###   phase to the download site. As in the "pull" phase, the AWS CLI[0] tools
###   are used perform the synchronization.
###
###   [0] https://aws.amazon.com/cli/
###

set -e
set -x
source "./jenkins/build/common.sh"

# Set the dry-run, -d, option to a default of NO.
declare DRYRUN=no

# Gather and parse all of the expected command-line parameters.
while getopts ':d-:' OPTNAME; do
  case $OPTNAME in
    d) DRYRUN=yes ;;
    -)
      case $OPTARG in
        action|action=*) optparse ACTION "$@";;
        version|version=*) optparse VERSION "$@" ;;
        *) usage "illegal option -- ${OPTARG%=*}" ;;
      esac
      ;;
    :)  usage "option requires an argument -- $OPTARG" ;;
    \?) usage "illegal option -- $OPTARG" ;;
  esac
done

shift $((OPTIND - 1))

# Affirm that the required arguments were passed via command line.
[[ -n $ACTION ]] || usage 'must specify an action'
[[ -n $VERSION ]] || usage 'must specify a version'

# Affirm that the <action> argument provided is expected.
case $ACTION in
  release-2-testing)
    TYPE="build"
    SOURCE="release"
    TARGET="testing"
    ;;
  *)
    die "Unknown action $ACTION"
    ;;
esac

SYNC_EXTRA_ARGS=()

if [[ $DRYRUN = 'yes' ]]; then
  SYNC_EXTRA_ARGS+=(--dryrun)
fi

# Create an empty directory to use for sync with s3.
mkdir -p incoming
rm -rf incoming/*
mkdir -p incoming/${TARGET}

printf '>>> synchronizing local copy with latest changes from testing\n'
printf '\n'

# Use the AWS CLI sync command to make a local copy of the following AWS S3 asset:
#   s3://nr-downloads-private/75ac22b116/c_agent/
aws s3 sync \
    "${SYNC_EXTRA_ARGS[@]}" \
     '--delete' \
    '--exclude=*' \
    '--include=c_agent/*' \
    '--exclude=*.DS_Store' \
    's3://nr-downloads-private/75ac22b116' \
    "./incoming"


# Thanks to the jenkins job, a copy of libnewrelic*.tgz should be sitting in the
# current directory.  Move it to the directory being used for the sync.  If there are
# more than 1 libnewrelic*.tgz files that have been built by the upstream jenkins
# job, they will be copied into the directory.
cp libnewrelic*.tgz incoming/c_agent

# Create a sha1sum for any tgz that does not have one or
# is no longer valid. Drop into folder so sha1sum files
# don't have full filepath. Return to top directory once
# we're done.
cd incoming/c_agent
for f in *.tgz
do
    sha1sum $f > temp.sha1sum

    # In case of a failed comparison ensure a 0 return value so that Jenkins
    # does not abort.
    compare=$(cmp $f.sha1sum temp.sha1sum 2>/dev/null || true)

    if [ ! -f $f.sha1sum ]
    then
      mv temp.sha1sum $f.sha1sum
    elif [ "$compare" != "" ]
    then
      mv temp.sha1sum $f.sha1sum
    fi

    rm -f temp.sha1sum 2> /dev/null
done
cd ../..



printf '>>> pushing changes to S3...\n'
printf '\n'

# Use the AWS CLI sync command to push the local copy of ./incoming/ to
# the stated s3 location.
aws s3 sync \
    "${SYNC_EXTRA_ARGS[@]}" \
    '--exclude=*' \
    '--include=c_agent/*' \
    '--exclude=*.DS_Store' \
    "./incoming" \
    's3://nr-downloads-private/75ac22b116'
