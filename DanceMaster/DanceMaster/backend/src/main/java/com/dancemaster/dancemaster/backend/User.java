package com.dancemaster.dancemaster.backend;

import com.google.appengine.labs.repackaged.org.json.JSONArray;
import com.google.appengine.labs.repackaged.org.json.JSONException;
import com.google.appengine.labs.repackaged.org.json.JSONObject;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;

/**
 * A class to store information about each user connected to the server
 */
public class User {
    // Instance variables
    public static final String USER_PARENT_ENTITY_NAME = "UserParent";
    public static final String USER_PARENT_KEY_NAME = "UserParent";
    public static final String USER_ENTITY_NAME = "User";

    public static final String FIELD_NAME_USERNAME = "username";
    public static final String FIELD_NAME_REGID = "regID";
    public static final String FIELD_NAME_FRIENDS = "friends";
    public static final String FIELD_NAME_POINTS = "points";

    private String username;
    private String regID;
    private ArrayList<String> friends;
    private HashMap<String, Integer> points;


    //************ Methods to get and set parameters ************//
    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getRegID() {
        return regID;
    }

    public void setRegID(String regID) {
        this.regID = regID;
    }

    public ArrayList<String> getFriends() {
        // Initialize the list if necessary
        if (friends == null) {
            friends = new ArrayList<>();
        }
        return friends;
    }

    public void setFriends(ArrayList<String> friends) {
        this.friends = friends;
    }

    public void addFriend(String friend) {
        // Initialize the list if necessary
        if (friends == null) {
            friends = new ArrayList<>();
        }
        friends.add(friend);
    }

    public HashMap<String, Integer> getPoints() {
        // Initialize the list if necessary
        if (points == null) {
            points = new HashMap<>();
        }
        return points;
    }

    public void setPoints(HashMap<String, Integer> points) {
        this.points = points;
    }


    /** Return if the given user's username is in the non-null list of friends */
    public boolean isFriend(User other) {
        if (friends != null && other.getUsername() != null) {
            if (friends.contains(other.getUsername())) {
                return true;
            }
        }
        return false;
    }


    public void addPoint(String date, Integer point) {
        // Initialize the list if necessary
        if (points == null) {
            points = new HashMap<>();
        }
        points.put(date, point);
    }

    /** Sum up how many points the user has earned this week. */
    public String pointsPerWeek() {
        if (points == null) {
            return "0";
        }

        int sum = 0;        // Initialize the sum variable
        for (int i = 0; i < 7; i++) {       // For the past seven days
            Calendar calendar = Calendar.getInstance();
            calendar.add(Calendar.DAY_OF_MONTH, -i);
            calendar.set(Calendar.HOUR_OF_DAY, 0);
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);

            Date date = calendar.getTime();
            String dateString = date.toString();
            if (points.get(dateString) != null) {
                sum += points.get(dateString);
            }
        }
        return Integer.toString(sum);
    }


    /** Return a string that has the points for the past seven days. */
    public String JSONofPoints() {
        if (points == null) {
            return "0";
        }
        try {
            JSONArray ja = new JSONArray();
            // For all the dates in the HashMap
            for (String date : points.keySet()) {
                ja.put(new JSONObject().put("date", date));
                ja.put(new JSONObject().put("points", Integer.toString(points.get(date))));
            }
            return ja.toString();

        } catch (JSONException e) {
            e.printStackTrace();
        }
        return null;
    }


    /** Parse out the information from the String and put it into a HashMap */
    public void setPoints(String points) {
        if (points.equals("0")) {
            this.points = null;
        }
        else {
            this.points = new HashMap<>();
            try {
                JSONArray ja = new JSONArray(points);
                JSONObject jo;

                // Get the data from each JSON object and add it to the list
                for (int i = 0; i < ja.length(); i += 2) {
                    jo = ja.getJSONObject(i);
                    String date = jo.getString("date");
                    jo = ja.getJSONObject(i + 1);
                    Integer point = Integer.parseInt(jo.getString("points"));

                    this.points.put(date, point);
                }

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }
}
