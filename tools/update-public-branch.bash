#!/bin/bash
#

set -e

if [ "$1" = "-h" -o "$1" = "--help" -o -z "$1" ] ; then
    echo -e \
	 "Usage: $0 <branchname>\n" \
	 "\n" \
         "This scripts creates or updates a C SDK release branch which can\n" \
         "then be synced to the public C SDK repository.\n" \
	 "\n" \
	 "This script has to be executed in the root directory of a Git\n" \
	 "repository clone." >&2
    exit 1
fi

if [ ! -d ".git" ] ; then
    echo -e \
	 "This script has to be executed in the root directory of a Git\n" \
	 "repository clone." >&2
    exit 1
fi

branch="$1"

logfile=$(basename $0).log
exec > $logfile

# The branch which will hold the final state that can be syned to the public
# C SDK repository.
#
release_branch="public-branches/$branch"

# The tag of the commit on which the last sync to the release branch was based.
#
last_release_tag="$release_branch/last"

# A temporary branch used to do squashing and filtering.
#
temp_branch="$release_branch-tmp"

# Determine if a baseline exists (if this branch has already been synced
# previously). This is done by checking for the existence of a release branch
# and a release tag.
#
baseline_exists() {
    git rev-parse $last_release_tag origin/$release_branch > /dev/null 2>&1
}

# Make sure all temporary tags and branches are removed on exit. Also make sure
# to remove them before we start. Also make sure we end up on the original
# branch when the script exits.
#
remove_temporary() {
    git checkout $branch
    git branch -D $temp_branch || true
}
trap remove_temporary EXIT
remove_temporary


# Step 1
# ======
#
# Create the temporary branch and reset it to the state of the last sync. If
# no sync has happened yet (if no release tag exists), reset the temporary
# branch to the state of the target branch.
#
git branch -f $temp_branch
git checkout $temp_branch

if baseline_exists ;  then
    git reset --hard $last_release_tag
else
    git reset --hard origin/$branch
fi


# Step 2
# ======
# Squash the temporary branch, which now holds the contents of the last sync. 
# This squashed commit will serve as baseline for further rebasing.
#
baseline_commit=$(git commit-tree 'HEAD^{tree}' -m "Initialize $branch")
git reset $baseline_commit


# Step 3
# ======
# Merge new commits made since the last sync.
#
if baseline_exists ;  then
    git reset --hard origin/$branch
    git rebase --onto $baseline_commit $last_release_tag $temp_branch
fi


# Step 4
# ======
# Filter out unwanted files. This is done based on the whitelist below. 
# Directories in the whitelist are recursively expanded, based on this list 
# and all files known to Git a blacklist is created. Finally, all files in 
# this blacklist are purged from the Git history.
#
# Files in the list force_black_list are forcefully filtered out, even if they
# match a pattern in the whitelist.
#

raw_white_list=".clang-format
.gitignore
CHANGELOG.md
combine.mri
Doxyfile
examples
GUIDE.md
include
LICENSE.txt
make
Makefile
README.md
src
tests
vendor/cmocka
vendor/Makefile
vendor/newrelic/axiom
vendor/newrelic/LICENSE.txt
vendor/newrelic/make
vendor/newrelic/Makefile
vendor/newrelic/src
VERSION"

force_black_list="vendor/newrelic/src/newrelic/collector/compile_certs.go
vendor/newrelic/src/newrelic/collector/certs_newrelic.go
vendor/newrelic/make/secrets.mk"

blacklist=$(
    comm -23 -- \
	<(git ls-files | sort) \
	<(
	    for f in $raw_white_list ; do
		find $f -type f
	    done | sort
	 )
)

if [ ! -z "$blacklist" ] ; then
    git filter-branch -f --index-filter \
	"git rm -r -f --ignore-unmatch --cached -- $(echo $blacklist $force_black_list)" \
	--prune-empty
fi

# Step 4
# ======
# Merge new commits onto the release branch and reset the release tag. 
# Merge only commits that remain on the temporary branch after the filtering.
#
if baseline_exists ; then
    baseline_commit=$(git log $temp_branch --oneline | tail -1 | sed 's/ .*//g')
    git rebase --onto origin/$release_branch $baseline_commit $temp_branch
    git checkout $release_branch
    git reset --hard $temp_branch
    git tag -f ${last_release_tag}-1 $last_release_tag
else
    git branch -f $release_branch $temp_branch
    git tag -f ${last_release_tag}-1 origin/$branch
fi

git tag -f $last_release_tag origin/$branch

# Step 5
# ======
# Write a public.json to save the branch configuration.
git checkout $branch
echo "{" > public.json
echo " \"$branch\" : \"$(git rev-parse $branch)\"," >> public.json
echo " \"$release_branch\" : \"$(git rev-parse $release_branch)\"," >> public.json
echo " \"$last_release_tag\" : \"$(git rev-parse $last_release_tag)\"," >> public.json
echo " \"$last_release_tag-1\" : \"$(git rev-parse ${last_release_tag}-1)\"" >> public.json
echo "}" >> public.json
git add public.json
git commit -m "Update public branch" public.json
