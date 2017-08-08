package com.dancemaster.dancemaster;

/** Custom class to store a String and an integer.
 * Used to store your own points (Dates as Strings to points per day, and
 * used to store your friends' points (usernames to weekly points) */
public class ScorePair {
    // Instance variables
    public String key;
    public Integer val;

    // Constructor
    public ScorePair(String newKey, Integer newVal) {
        key = newKey;
        val = newVal;
    }


    @Override
    public boolean equals(Object other) {
        if (!(other instanceof ScorePair)) {
            return false;
        }
        ScorePair otherSP = (ScorePair) other;
        return otherSP.key.equals(this.key);
    }
}