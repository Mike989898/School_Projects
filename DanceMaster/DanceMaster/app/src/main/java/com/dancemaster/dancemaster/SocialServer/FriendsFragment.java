package com.dancemaster.dancemaster.SocialServer;

import android.app.Fragment;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TableLayout;
import android.widget.TableRow;

import com.dancemaster.dancemaster.FancyUI.GoodDogTextView;
import com.dancemaster.dancemaster.MainActivity;
import com.dancemaster.dancemaster.R;
import com.dancemaster.dancemaster.ScorePair;

import java.util.ArrayList;
import java.util.Locale;

/**
 * FriendsFragment.java is the social tab of the MainActivity
 * It displays a table of the user's friends in descending order
 * based on their points this week. There is a button to add
 * friends at the bottom of the screen.
 */
public class FriendsFragment extends Fragment {
    // Instance variables
    private static TableLayout table;
    private LayoutInflater inflater;
    private static final String FRIENDS_HEADER = "Friends!";
    private View view;


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        this.inflater = inflater;
        view = inflater.inflate(R.layout.friends_fragment, container, false);
        table = (TableLayout) view.findViewById(R.id.table);
        fillTable();
        noFriends();
        return view;
    }


    @Override
    public void onResume() {
        table.removeAllViews();
        fillTable();
        noFriends();
        super.onResume();
    }


    // Populate the table with data on friends' weekly points
    private void fillTable() {
        if (MainActivity.friendsPoints != null && !MainActivity.friendsPoints.isEmpty()) {
            // Create a new row of the table for every friend in your friend list
            fillRow(FRIENDS_HEADER);

            ArrayList<ScorePair> unsortedFriends = new ArrayList<>();
            for (String friend : MainActivity.friendsPoints.keySet()) {
                unsortedFriends.add(new ScorePair(friend,
                        MainActivity.friendsPoints.get(friend)));
            }

            ArrayList<ScorePair> sortedFriends = sortFriends(unsortedFriends);
            for (ScorePair friendPair : sortedFriends) {
                fillRow(friendPair.key);
            }
        }
    }

    /** Method to sort a list of Score Pairs (String friend to Integer score)
     * in descending order by their scores. */
    private static ArrayList<ScorePair> sortFriends(ArrayList<ScorePair> unsortedFriends) {
        // Initialize the array list to be returned and declare some starting variables.
        ArrayList<ScorePair> sortedFriends = new ArrayList<>();
        int biggestScore = Integer.MIN_VALUE;
        int biggestIndex = -1;
        int score;
        ScorePair temp;

        // Selection sort
        for (int i = 0; i < unsortedFriends.size(); i++) {
            for (int j = i; j < unsortedFriends.size(); j++) {
                score = unsortedFriends.get(j).val;

                if (score > biggestScore) {
                    biggestIndex = j;
                    biggestScore = score;
                }
            }
            sortedFriends.add(new ScorePair(unsortedFriends.get(biggestIndex).key, unsortedFriends.get(biggestIndex).val));

            // Swap items.
            temp = new ScorePair(unsortedFriends.get(i).key, unsortedFriends.get(i).val);
            unsortedFriends.set(i, new ScorePair(unsortedFriends.get(biggestIndex).key, unsortedFriends.get(biggestIndex).val));
            unsortedFriends.set(biggestIndex, temp);

            biggestScore = Integer.MIN_VALUE;
        }
        return sortedFriends;
    }


    /** If the user has no friends, display a text view that says
     * they don't have any friends. Otherwise get rid of the text view. */
    private void noFriends() {
        GoodDogTextView noFriends = (GoodDogTextView) view.findViewById(R.id.no_friends);
        RelativeLayout.LayoutParams llp = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);

        if (MainActivity.friendsPoints == null || MainActivity.friendsPoints.isEmpty()) {
            noFriends.setText(R.string.no_friends);
            noFriends.setTextColor(Color.parseColor("#FF0099"));

            llp.setMargins(0, 300, 0, 0);
            noFriends.setGravity(Gravity.CENTER_HORIZONTAL);
            noFriends.setLayoutParams(llp);

        } else {
            noFriends.setText("");
            noFriends.setVisibility(View.GONE);
            llp.setMargins(0, 0, 0, 0);
        }
    }


    /** Given a String that is the friend's username, fill in the table with
     * that friend's username and their points. */
    private void fillRow(String friend) {
        // Create a new row of the table from the template
        View row = inflater.inflate(R.layout.table_row, null, false);

        // Set the username and points fields of the row
        GoodDogTextView user = (GoodDogTextView) row.findViewById(R.id.user_title);
        user.setText(friend);
        user.setGravity(Gravity.CENTER_HORIZONTAL);

        GoodDogTextView points = (GoodDogTextView) row.findViewById(R.id.user_points);
        if (friend.equals(FRIENDS_HEADER)) {        // Header row
            user.setTextColor(Color.parseColor("#07a925"));         // Green
            points.setText(getString(R.string.points));
            points.setTextColor(Color.parseColor("#07a925"));

            user.setBackgroundColor(Color.parseColor("#37c4b6")); // Darker blue
            points.setBackgroundColor(Color.parseColor("#FF66D9"));   // Darker pink
        } else {        // Normal friend row
            user.setTextColor(Color.parseColor("#a712f2"));
            points.setText(String.format(Locale.US, "%1$d",
                    MainActivity.friendsPoints.get(friend)));
            points.setTextColor(Color.parseColor("#a712f2"));       // Purple
        }
        points.setTextSize(30);     // Large text size

        // Set the layout weight so that two distinct columns are displayed
        TableRow.LayoutParams param = new TableRow.LayoutParams(
                TableRow.LayoutParams.WRAP_CONTENT,
                TableRow.LayoutParams.MATCH_PARENT, 1.0f);
        user.setLayoutParams(param);
        points.setLayoutParams(param);

        // Add the row to the table
        table.addView(row);
    }
}