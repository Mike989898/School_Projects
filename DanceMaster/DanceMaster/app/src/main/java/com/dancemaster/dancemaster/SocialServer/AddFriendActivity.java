package com.dancemaster.dancemaster.SocialServer;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.dancemaster.dancemaster.GLOBALS;
import com.dancemaster.dancemaster.R;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * AddFriendActivity.java pops up after clicking the
 * "Add Friends" button from the Friends Fragment.
 */
public class AddFriendActivity extends Activity {
    // Instance variables
    private AddFriendBroadcastReceiver broadcastReceiver;
    private String username;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_friend);

        // Initialize and register the broadcast receiver
        broadcastReceiver = new AddFriendBroadcastReceiver(this);
        LocalBroadcastManager lbm = LocalBroadcastManager.getInstance(this);
        lbm.registerReceiver(broadcastReceiver, new IntentFilter(GLOBALS.ACTION_ADD));
        lbm.registerReceiver(broadcastReceiver, new IntentFilter(GLOBALS.ACTION_DATA));
    }


    /** When the user clicks on the Follow button, see if the entered username
     * is valid, i.e. not themselves, existing, and not already being followed
     * by this user, and add this user to your list of friends.
     * Start an AsyncTask to connect to the server. */
    public void onAddFriendClicked(View v) {
        username = ((EditText) findViewById(R.id.friend_username)).getText().toString();

        // This user cannot follow themselves.
        if (username.equals(GLOBALS.preferences.getString(getString(R.string.username), ""))) {
            Toast.makeText(getApplicationContext(),
                    getString(R.string.narcissist),
                    Toast.LENGTH_SHORT).show();

        // If the user has actually entered a username, communicate with the server.
        } else if (!username.equals("")) {
            try {
                new AddUsername().execute(username);
            } catch (Exception e) {
                Toast.makeText(getApplicationContext(), "Unable to connect to cloud!",
                        Toast.LENGTH_SHORT).show();
            }
        }
    }

    /** When the cancel button is clicked, exit out of this activity,
     * returning to the Friends Fragment. */
    public void onCancelClicked(View v) {
        // Close the activity
        finish();
    }


    /**
     * An AsyncTask to send the proposed username to the service
     * to add it to the user's list of friends.
     */
    private class AddUsername extends AsyncTask<String, Void, Void> {
        // Send a message to the server with this phone's registration
        // id and proposed username
        @Override
        protected Void doInBackground(String... username) {
            try {
                // Create the map to send to the server
                Map<String, String> map = new HashMap<>();
                map.put(GLOBALS.MESSAGE_KEY_USER, username[0]);
                map.put(GLOBALS.MESSAGE_KEY_REGID, GLOBALS.regID);

                // Send this message to the server
                ServerMessenger.send(GLOBALS.SERVER_ADDR + "/addFriend.do", map);
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }


    /**
     * A BroadcastReceiver to receive the response from the service
     */
    private class AddFriendBroadcastReceiver extends BroadcastReceiver {
        private Activity act;

        public AddFriendBroadcastReceiver(Activity act) {
            this.act = act;
        }

        // Receive a broadcast with response from service
        @Override
        public void onReceive(Context context, Intent intent) {
            // Get the server's result from the intent
            String result = "";
            if (intent.getAction().equals(GLOBALS.ACTION_ADD)) {
                result = intent.getStringExtra(GLOBALS.MESSAGE_CODE_ADD);
            } else if (intent.getAction().equals(GLOBALS.ACTION_DATA)) {
                result = intent.getStringExtra(GLOBALS.MESSAGE_CODE_DATA);
            }

            if (result.equals("none")) {}
            else if (result.equals("already_added")) {}
            else if (result.equals("done")) {
                try {  // Reload the friends data
                    new GcmGetDataMainTask().execute();
                } catch (Exception e) {
                    Toast.makeText(getApplicationContext(),
                            R.string.ui_toast_connection_unsuccessful,
                            Toast.LENGTH_SHORT).show();
                }
            } else {        // We're coming back from ACTION_DATA
                act.finish();   // Close the activity
            }
        }
    }
}