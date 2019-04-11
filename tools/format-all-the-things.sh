#!/bin/bash

basedir=${1:-.}

format_files_matching() {
    pattern=$1
    shift

    for f in $(find $basedir -name $pattern) ; do
	if ! cmp $f <($* $f) >/dev/null 2>&1
	then
	    echo "Diffs in $f"
	    # The next line is a matter of personal taste.
	    # vimdiff $f <($* $f)
	fi
    done
}

format_files_matching "*.[ch]" clang-format-5.0 --style=file
format_files_matching "*.go" gofmt
