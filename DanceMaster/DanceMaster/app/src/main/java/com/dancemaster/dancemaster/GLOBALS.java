package com.dancemaster.dancemaster;

import android.content.SharedPreferences;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Set;

/**
 * A class to contain all constant variables
 * and handy methods used in the frontend side of this project.
 */
public abstract class GLOBALS {
    // Constants for accessing the server
    public static final String SERVER_ADDR =
            "https://maximal-shadow-132321.appspot.com";  // Deployed
    public static final String SENDER_ID = "469578628418";

    public static String regID;
    public static SharedPreferences preferences;

    // Constants for sending messages to, and receiving messages from, the server
    public static final String MESSAGE_KEY_USER = "user";
    public static final String MESSAGE_KEY_REGID = "regID";
    public static final String MESSAGE_KEY_DATE = "date";
    public static final String MESSAGE_KEY_POINTS = "points";

    // Constants for sending broadcasts
    public static final String RESULT_CODE = "result";
    public static final String ACTION_USERNAME = "username";
    public static final String ACTION_ADD = "add";
    public static final String ACTION_DATA = "data";
    public static final String MESSAGE_CODE_USERNAME = "username";
    public static final String MESSAGE_CODE_ADD = "add";
    public static final String MESSAGE_CODE_DATA = "data";
    public static final String MESSAGE_CODE_UPDATE = "update";

    // Variables dealing with progress tracking and points.
    public static final int numPointsPerVideo = 10;
    public static HashMap<String, Integer> dailyPoints = new HashMap<>();

    public static final String[] VIDEO_KEYS =
            {"7uaIClJdOxU", "sLEIT1HD-3A", "HuYhK9NHLbQ", "2pJThFCva3c",
                    "grSaqL3Q9qs", "cTe1w5cD2PI", "Aeonkbk_Kb8", "x2yFBkyyNEc",
                    "CH259YU_660", "5LvWTAL5z4Y", "tsfm_z3Yf9M", "uEXXe8Sn8r8"};


    /** CSV writing and reading functions follow. */
    /**
     * Appends a given string to a given CSV file
     * @param f: File. The CSV file to be written to.
     * @param data: String. The String to write to f
     */
    public static void writeToCSV(File f, String data) {
        FileOutputStream fos;

        try {
            // Boolean second parameter: if true, append to file
            fos = new FileOutputStream(f, false);
            fos.write(data.getBytes());

            // Close stream
            fos.flush();
            fos.close();
        } catch (IOException e) {   // Catch any errors
            e.printStackTrace();
        }
    }


    /**
     * Reads stress entries from the CSV file.
     * @param f: File. The file to read from.
     * @return ArrayList<String[]>. Each element is a stress entry
     * whose first element is the time stamp and
     * whose second element is the stress level.
     */
    public static List<String[]> readFromCSV(File f) {
        // Catch the edge case where the file does not exist
        if (!f.exists()) {
            return null;
        }

        // CSVLines will hold the stress entries
        List<String[]> CSVLines = new ArrayList<>();

        FileInputStream fis;
        try {
            fis = new FileInputStream(f);
            BufferedReader reader = new BufferedReader(new InputStreamReader(fis));
            String line;    // A line from the CSV file

            // Read in the CSV file a line at a time
            while ((line = reader.readLine()) != null) {
                // Create an array of Strings separated by the comma
                String[] entry = line.split(",");
                // Add the entry to the list of entries
                CSVLines.add(entry);
            }

            // Close any streams
            reader.close();
            fis.close();
        } catch (IOException e) {   // Catch any errors
            throw new RuntimeException("Error on read:" + e);
        }

        return CSVLines;
    }


    /** All subsequent methods write to and interact with the HashMap. */
    /** Create a hash map from a list of String arrays. */
    public static HashMap<String, Integer> createHashMap(List<String[]> l) {
        HashMap<String, Integer> hm = new HashMap<>();

        String[] entry;
        for (int i = 0; i < l.size(); i++) {
            entry = l.get(i);
            hm.put(entry[0], Integer.parseInt(entry[1]));
        }
        return hm;
    }


    /** Write the contents of the HashMap to a CSV file. */
    public static void writeHashMapToCSV(HashMap<String, Integer> hm) {
        Set<String> keySet = hm.keySet();

        String stringToWrite = "";
        for (String key : keySet) {
            stringToWrite = stringToWrite + key + "," + hm.get(key) + "\n";
        }
        writeToCSV(new File(MainActivity.dir, "score_entries.csv"), stringToWrite);
    }


    /** Convert the hash map to readable contents for the graph. */
    public static List<String[]> convertHashMapToGraphData(HashMap<String, Integer> hm) {
        List<ScorePair> unsortedData = new ArrayList<>();
        Set<String> keySet = hm.keySet();

        for (String key : keySet) {
            unsortedData.add(new ScorePair(key, hm.get(key)));
        }
        fillInMissingDays(unsortedData);
        List<String[]> l = sort(unsortedData);
        return l;
    }


    /** Sort an unsorted List of ScorePair objects
     * based on the Strings which are Dates. Sort in
     * chronological order. */
    public static List<String[]> sort(List<ScorePair> unsorted) {
        List<String[]> sorted = new ArrayList<>();

        String[] toBeAdded;
        long smallestTime = Long.MAX_VALUE;
        int smallestIndex = -1;
        ScorePair temp;
        Date d;
        long time;

        // Selection sort
        for (int i = 0; i < unsorted.size(); i++) {
            for (int j = i; j < unsorted.size(); j++) {
                d = stringToDate(unsorted.get(j).key);
                time = d.getTime();

                if (time < smallestTime) {
                    smallestIndex = j;
                    smallestTime = time;
                }
            }

            // Swap items
            toBeAdded = new String[2];
            toBeAdded[0] = unsorted.get(smallestIndex).key;
            toBeAdded[1] = Integer.toString(unsorted.get(smallestIndex).val);
            sorted.add(toBeAdded);

            temp = new ScorePair(unsorted.get(i).key, unsorted.get(i).val);
            unsorted.set(i, new ScorePair(unsorted.get(smallestIndex).key, unsorted.get(smallestIndex).val));
            unsorted.set(smallestIndex, temp);
            smallestTime = Long.MAX_VALUE;
        }
        return sorted;
    }


    /** Convert a string to a Date object */
    public static Date stringToDate(String sd) {
        SimpleDateFormat formatter = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.US);

        Date date = null;
        try {
            date = formatter.parse(sd);
        } catch (ParseException e) {
            e.printStackTrace();
        }

        return date;
    }


    /** To make the progress graph show up complete for the past seven days. */
    public static void fillInMissingDays(List<ScorePair> l) {
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.DAY_OF_MONTH, -7);

        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);

        Date d;
        for (int i = 0; i < 7; i++) {       // For the past seven days
            d = calendar.getTime();

            // Create an entry for that day if there wasn't one already
            if (!(l.contains(new ScorePair(d.toString(), 0)))) {
                l.add(new ScorePair(d.toString(), 0));
            }
            calendar.add(Calendar.DAY_OF_MONTH, 1);
        }
    }
}
