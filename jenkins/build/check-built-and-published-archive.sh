#!/bin/bash

# something in our jenkins instance (or ec2 node (or Dockerfile(s)))
# seems to strip `/bin` from the path, so we add itback
PATH=/bin:$PATH

# The URL paramater from the jenkins job, should be the URL to an
# agent tarball
URL_AGENT_TGZ=${URL_AGENT_TGZ:-http://example.com/foo.tgz}

# The file our daemon test will log to
LOGFILE="./logfile"

# The directory that we untar our files to
TARDIR="libnewrelic"

set -x
set -e

# downloads the archive file
download_archive () {
    curl -LO http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/c_agent/${1}
    curl -LO http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/c_agent/${1}.sha1sum

    $SHA_CMD -c ${1}.sha1sum
}

# Splits URL and grabs file name from last part
function set_filename_from_url() {
    IFS='/'
    read -ra PARTS <<< "$1"
    FILENAME=${PARTS[${#PARTS[@]}-1]}
}

# Sometimes sha1sum is available, sometimes shasum is.  Computers. ¯\_(ツ)_/¯
set_sha_command () {
    # We use the return code from which to determine if the file's there or not
    set +e
    SHA_CMD=sha1sum
    which $SHA_CMD
    if [ "$?" != "0" ]
    then
        SHA_CMD=shasum
    fi
    # go back to being strict
    set -e
}

# The main job that untars the file, compiles the test application
# starts the daemon, and then runs the program
main () {
    TARFILE=$1

    # Download and extract tarball
    # curl $TARFILE
    tar -xf $TARFILE $TARDIR

    # Copy over test app to compile against
    cp test_app.c $TARDIR

    # Compile test app
    gcc -o "$TARDIR/test_app" "$TARDIR/test_app.c" -L$TARDIR -lnewrelic -lpcre -pthread

    echo "DEBUG: $LOGFILE"
    # Start the Daemon as a background process, give it a chance to start up
    { rm -f "$LOGFILE"; $TARDIR/bin/newrelic-daemon -f -logfile "$LOGFILE" -loglevel debug; } &
    sleep 10

    # Start the test_app
    { $TARDIR/test_app; } &

    # Don't continue until one of the two background jobs finish (the test_app)
    wait -n
    rc=$?; if [[ $rc != 0 ]]; then echo "Daemon or test_app failed to run"; exit $rc; fi

    sleep 60

    echo "Terminating child process(es)!"
    pkill -P $$
    rc=$?; if [[ $rc != 0 ]]; then echo "Failed to kill child processes"; exit $rc; fi

    # Grep URL from logfile
    url=`grep -oP '(?<=Reporting to:[[:space:]])(http|https)://[^"]+' "$LOGFILE"`

    printf "\nTest app reported to URL: $url\n"

    printf "Running: cat $LOGFILE"
    cat "$LOGFILE"
}

# sets the $FILENAME global by parsing the URL
set_filename_from_url $URL_AGENT_TGZ

# sets a $SHA_CMD global that checks for sha1sum vs. shasum
set_sha_command

# downloads a file from the S3 buckets, validates its shasum FILENAME
# http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116
download_archive $FILENAME

# untars the archive, compiles a test program, runs it
main $FILENAME
