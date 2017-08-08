#!/bin/bash 
#!bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

check_out () {
	#echo -e "\n>$3"
	#cat out.txt
	check=$( diff out.txt expected_outputs/$2 )

	if [[ -z $check ]]; then
		echo -e "\nTest $1 ${GREEN}Passed!${NC}"
	else
		echo -e "\nTest $1 ${RED}Failed${NC}. Diff is \n" "$check" 
		if [[ "$first" -eq "1" ]]; then
			vimdiff out.txt "expected_outputs/$2"
			first="0"
		fi
	fi

	return 0

}

cd ..
make clean
make 

if [[ ! $? -eq "0" ]]; then
	echo "Failed to make"
	exit 1	
fi

cd indexer

make indextest

[ -d testDir ] && rm -r testDir

first=1;

./indexer ./testDir expected_outputs/index.txt blobloblo > out.txt

check_out "Too many Arguments" "too_many_args.txt" "./indexer"

./indexer ./testDir expected_outputs/index.txt blobloblo > out.txt

check_out "Too few arguments" "no_args.txt" "./indexer"

./indexer ./_notarealdir expected_outputs/index.txt  > out.txt

check_out "Too few arguments" "inv_dir.txt" "./indexer"

# We assume crawler works. 
../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../indexer/testDir 1 

echo "crawled to depth 1"

./indexer ./testDir expected_outputs/index.txt

./indexsort.sh expected_outputs/index.txt > out.txt

check_out "Indexer base test" "depth_0.txt" "./indexer"

indextest.sh index.txt
