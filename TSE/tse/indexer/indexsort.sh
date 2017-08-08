#!/bin/bash
#
# indexsort - a little script to sort a TSE index file:
#   lines are sorted alphabetically by word,
#   pairs within each line are sorted numerically by docID.
#
# depends on a companion script indexsort.awk
# 
# David Kotz, April 2016

awkhelper=indexsort.awk

if [ $# != 1 ]; then
    echo usage: $0 indexfile
    exit 1
fi

if [ ! -r "$1" ]; then
    echo $0: input file "'$1'" is not readable.
    exit 2
fi

gawk -f $awkhelper "$1" | sort
