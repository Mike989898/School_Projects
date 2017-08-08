#!/bin/bash
#
# indextest - load and save an index, and verify output
#
# usage: indextest indexFilename
# 
# load that index, save it to a temporary file 'indexFilename.tmp';
# sort both, compare the sorted indices to see if they are the same.
#
# David Kotz, April 2016
#

if [ $# != 1 ]; then
    echo usage: indextest indexFilename
    exit 1
else
    index="$1"
    tmpindex="$index.tmp"
fi

# clean up at end, and on any interruption.
trap "rm -f $index.sorted $tmpindex $tmpindex.sorted" EXIT

echo running indextest to load "$index" and create "$tmpindex"
./indextest "$index" "$tmpindex" || exit 2

if cmp --silent "$index" "$tmpindex" ; then
    echo perfect: files are identical
    exit 0
fi

echo sort  "$index" to create  "$index.sorted"
./indexsort.sh "$index" >  "$index.sorted" || exit 3

echo sort  "$tmpindex" to create  "$tmpindex.sorted"
./indexsort.sh "$tmpindex" >  "$tmpindex.sorted" || exit 4

echo diff  "$index.sorted" "$tmpindex.sorted"
diff  "$index.sorted" "$tmpindex.sorted"  || exit 5
