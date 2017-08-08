package com.dancemaster.dancemaster.SocialServer;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.support.v4.content.WakefulBroadcastReceiver;

/**
 * A class to receive messages from the server
 */
public class GcmBroadcastReceiver extends WakefulBroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        // Specify the class of the service that will handle the intent
        ComponentName name = new ComponentName(context.getPackageName(),
                GcmIntentService.class.getName());

        // Start the service
        startWakefulService(context, (intent.setComponent(name)));
        setResultCode(Activity.RESULT_OK);
    }
}
