# CS50 Lab 4: The Crawler

##Testing
The main testing takes place in the bash script **testscript.sh**

This script checks several error conditions, including:
* No seed url is provided.
* Invalid seed_url
* invalid depth provided

and checks the output against several files stored in the **expected_outputs** directory.

I then runs checks at two different urls at depth of 0, 1, and 2, and compares the debugging output with expected results. Doing this at depth 3 takes too long

---
### Manual Testing:

We also made sure to test the expected output by looking at the various files and making sure that the output was correct.

We ran valgrind to look for memory leaks, and there are none when we ran it.
