package com.dancemaster.dancemaster.SocialServer;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
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
 * CreateUsernameActivity is the first activity to show up
 * when the user first opens the app. After the user has
 * entered a username that is different from all other existing
 * users, the activity will not show up again.
 */
public class CreateUsernameActivity extends Activity {
    // Instance variables
    private UsernameBroadcastReceiver broadcastReceiver;
    private String username;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_create_username);

        // Connect to the cloud
        new GcmRegistrationTask(this).execute();

        // Initialize and register the broadcast receiver
        broadcastReceiver = new UsernameBroadcastReceiver();
        LocalBroadcastManager lbm = LocalBroadcastManager.getInstance(this);
        lbm.registerReceiver(broadcastReceiver, new IntentFilter(GLOBALS.ACTION_USERNAME));
    }


    /** When the user saves the entered username, start an AsyncTask
     * to check the server for if this username is already taken
     * in the list of usernames in the server's data store. */
    public void onSaveUserClicked(View v) {
        username = ((EditText) findViewById(R.id.create_username)).getText().toString();
        if (!username.equals("")) {
            try {
                new CheckUsername().execute(username);
            } catch (Exception e) {
                Toast.makeText(getApplicationContext(), "Unable to connect to cloud!",
                        Toast.LENGTH_SHORT).show();
            }
        }
    }


    /**
     * An AsyncTask to send the proposed username to the service
     * to make sure it's unique.
     */
    private class CheckUsername extends AsyncTask<String, Void, Void> {
        // Send a message to the server with this phone's registration id and proposed username
        @Override
        protected Void doInBackground(String... username) {
            try {
                // Create the map to send to the server
                Map<String, String> map = new HashMap<String, String>();
                map.put(GLOBALS.MESSAGE_KEY_USER, username[0]);
                map.put(GLOBALS.MESSAGE_KEY_REGID, GLOBALS.regID);

                ServerMessenger.send(GLOBALS.SERVER_ADDR + "/checkUser.do", map);
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }


    /**
     * A BroadcastReceiver to receive the response about the username from the service
     */
    private class UsernameBroadcastReceiver extends BroadcastReceiver {
        // Receive a broadcast with response from service
        @Override
        public void onReceive(Context context, Intent intent) {
            boolean usernameOK = Boolean.parseBoolean(
                    intent.getStringExtra(GLOBALS.MESSAGE_CODE_USERNAME));

            // Display a message and clear the text box if the username needs to be changed
            if (!usernameOK) {
                Toast.makeText(getApplicationContext(),
                        username + " " + getString(R.string.ui_toast_user_taken),
                        Toast.LENGTH_SHORT).show();
                ((EditText) findViewById(R.id.create_username)).setText("");

            // Otherwise, save the username and finish the activity
            } else {
                // Save the username to the shared preferences
                SharedPreferences preferences = GLOBALS.preferences;
                SharedPreferences.Editor editor = preferences.edit();
                editor.clear();

                String username = ((EditText) findViewById(R.id.create_username))
                        .getText().toString();
                editor.putString(getString(R.string.username), username);

                editor.apply();
                finish();       // Close the activity
            }
        }
    }
}
