package com.dancemaster.dancemaster.SocialServer;

import android.content.Context;
import android.os.AsyncTask;
import android.widget.Toast;

import com.dancemaster.dancemaster.GLOBALS;
import com.dancemaster.dancemaster.R;
import com.dancemaster.dancemaster.backend.registration.Registration;
import com.google.android.gms.gcm.GoogleCloudMessaging;
import com.google.api.client.extensions.android.http.AndroidHttp;
import com.google.api.client.extensions.android.json.AndroidJsonFactory;

import java.io.IOException;

/**
 * An AsyncTaskclass to register to the server
 */
public class GcmRegistrationTask extends AsyncTask<Void, Void, Boolean> {
    private Registration registration = null;
    private GoogleCloudMessaging gcm;
    private Context context;

    // Constructor to save context
    public GcmRegistrationTask(Context context) {
        this.context = context;
    }

    // Register to the server
    @Override
    protected Boolean doInBackground(Void... params) {
        // Create a new registration if necessary
        if (registration == null) {
            // Create the builder
            Registration.Builder builder = new Registration.Builder(
                    AndroidHttp.newCompatibleTransport(),
                    new AndroidJsonFactory(), null);
            builder.setRootUrl(GLOBALS.SERVER_ADDR + "/_ah/api/");
            // Create the registration from the builder
            registration = builder.build();
        }

        // Register to the server using the registration object
        try {
            // Get a new instance of the server if necessary
            if (gcm == null) {
                gcm = GoogleCloudMessaging.getInstance(context);
            }
            // Register to the server
            String regID = gcm.register(GLOBALS.SENDER_ID);
            registration.register(regID).execute();

            GLOBALS.regID = regID;      // Save the registration ID
        } catch (IOException e) {
            return false;
        }
        return true;
    }


    @Override
    protected void onPostExecute(Boolean connectionSuccessful) {
        if (connectionSuccessful) {
            Toast.makeText(context, R.string.ui_toast_connection_successful, Toast.LENGTH_LONG).show();

            // If there is a saved username and the connection in registered,
            // then download friend data
            String username = GLOBALS.preferences.getString("Username", "");
            if (username != null && !username.equals("")) {
                new GcmGetDataMainTask().execute();
            }

        } else {        // Display toast with result
            Toast.makeText(context, R.string.ui_toast_connection_unsuccessful, Toast.LENGTH_LONG)
                    .show();
        }
    }
}