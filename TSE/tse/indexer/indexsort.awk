# Sort each line of a TSE index file.
# Not very efficient: we write each line to a temp file, one pair per line,
# then run 'sort' to print that file in numerically sorted order.
#

BEGIN { 
    tmp = "indexsort.tmp";
}

# for each input line, 
{
    # print the word
    printf "%s ", $1;

    # print the pairs to a file
    for (i = 2; i < NF; i += 2)
	print $i, $(i+1) > tmp

    # sort that file to the output and convert newlines back to spaces
    system("sort -n " tmp " | tr '\\n' ' ' ");
    print "";

    # close the temp file so it will be overwritten next time
    close(tmp);
}

END {
    system("rm -f " tmp);
}

