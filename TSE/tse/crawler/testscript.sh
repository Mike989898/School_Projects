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

make clean
make 
echo "make clean; make"

if [[ ! $? -eq "0" ]]; then
	echo "Failed to make"
	exit 1	
fi

[ -d testDir ] && rm -r testDir

first=1;

./crawler > out.txt

check_out "No Arguments" "no_arg.txt" "./crawler"

./crawler http://notvalid/~cs50/index.html testDir 1 > out.txt

check_out "Invalid Seed URL" "invalid_seedURL.txt" "./crawler"

./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html testDir 1 > out.txt

check_out "Invalid Directory" "invalid_dir.txt" "./crawler"

./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html testDir pancakes > out.txt

check_out "Non-Numeric Max Depth" "non_numeric_max_depth.txt" "./crawler"

./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html testDir 13 > out.txt

check_out "Invalid Max Depth" "invalid_max_depth.txt" "./crawler"

mkdir testDir

./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html testDir 0 > out.txt

check_out "Depth 0" "depth_0.txt" "./crawler"


./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html testDir 1 > out.txt

check_out "Depth 1" "depth_1.txt" "./crawler"

./crawler http://old-www.cs.dartmouth.edu/~dfk testDir 2 > out.txt

check_out "Kotz Depth 2" "kotz_depth_2.txt" "./crawler"