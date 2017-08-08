package com.dancemaster.dancemaster.SocialServer;

import android.app.IntentService;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;

import com.dancemaster.dancemaster.GLOBALS;
import com.google.android.gms.gcm.GoogleCloudMessaging;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * A class to handle messages from the server
 */
public class GcmIntentService extends IntentService {
    // Constructor
    public GcmIntentService() {
        super("GcmIntentService");
    }

    // Method to handle the message from the server
    @Override
    protected void onHandleIntent(Intent intent) {
        Bundle extras = intent.getExtras();
        GoogleCloudMessaging gcm = GoogleCloudMessaging.getInstance(this);
        String messageType = gcm.getMessageType(intent);

        // If there is a message to read
        if (extras != null && !extras.isEmpty() &&
                GoogleCloudMessaging.MESSAGE_TYPE_MESSAGE.equals(messageType)) {
            // Parse the JSONArray
            try {
                JSONArray ja = new JSONArray(extras.getString("message"));
                JSONObject jo = ja.getJSONObject(0);
                Intent i;

                switch (jo.getString("method")) {
                    // Send a message to the profile activity
                    // with the result from the server
                    case GLOBALS.MESSAGE_CODE_USERNAME:
                        i = new Intent(GLOBALS.ACTION_USERNAME);
                        i.putExtra(GLOBALS.MESSAGE_CODE_USERNAME, ja.getJSONObject(1)
                                .getString(GLOBALS.RESULT_CODE));

                        LocalBroadcastManager.getInstance(this).sendBroadcast(i);
                        break;

                    // Send a message to the add friend activity
                    // with the result from the server
                    case GLOBALS.MESSAGE_CODE_ADD:
                        i = new Intent(GLOBALS.ACTION_ADD);
                        i.putExtra(GLOBALS.MESSAGE_CODE_ADD, ja.getJSONObject(1)
                                .getString(GLOBALS.RESULT_CODE));
                        LocalBroadcastManager.getInstance(this).sendBroadcast(i);
                        break;

                    // Send a message to the friend fragment
                    // with the result from the server
                    case GLOBALS.MESSAGE_CODE_DATA:
                        i = new Intent(GLOBALS.ACTION_DATA);
                        i.putExtra(GLOBALS.MESSAGE_CODE_DATA, ja.getJSONArray(1).toString());
                        LocalBroadcastManager.getInstance(this).sendBroadcast(i);
                        break;
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        GcmBroadcastReceiver.completeWakefulIntent(intent);
    }
}