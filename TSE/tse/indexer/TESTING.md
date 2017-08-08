### TESTING

# Compilation

Please compile using the `make` command in the root `tse` directory, not in indexer.

# Testing


## Manual Testing
There was a fair bit of manual testing, looking at the output files and then comparing that to the known input and making sure that the words we said were there actually were there.

## Automated Testing

The main aspect is the script `testscript.sh`, which runs several basic checks on arguments, with expected output in the `indexer/expected_outputs` folder. It also runs the crawler from a known url and depth (we assume that works) and then compares the output that indexer gets from that crawler run with what a previously saved run. We sort both files to compare.

We also use Proffesor Kotz's `indextest.sh` file.

We ran valgrind on both `index` and `indextest` and found that nothing was in use at the end. There were no memory leaks.