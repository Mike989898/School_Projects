package com.dancemaster.dancemaster.SocialServer;

import android.os.AsyncTask;

import com.dancemaster.dancemaster.GLOBALS;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * An AsyncTask to query the server to collect the friends data
 */
public class GcmGetDataMainTask extends AsyncTask<Void, Void, Void> {
    // Send a message to the server with this phone's registration id
    @Override
    protected Void doInBackground(Void... params) {
        try {
            // Create the map to send to the server
            Map<String, String> map = new HashMap<String, String>();
            map.put(GLOBALS.MESSAGE_KEY_REGID, GLOBALS.regID);

            ServerMessenger.send(GLOBALS.SERVER_ADDR + "/getData.do", map);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}