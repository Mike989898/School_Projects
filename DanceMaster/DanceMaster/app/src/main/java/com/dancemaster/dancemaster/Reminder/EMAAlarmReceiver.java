package com.dancemaster.dancemaster.Reminder;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.dancemaster.dancemaster.MainActivity;

/**
 * Created by varun on 1/20/15.
 * Adapted from StressMeter.
 */

public class EMAAlarmReceiver extends BroadcastReceiver {
    // Receive broadcast
    @Override
    public void onReceive(final Context context, Intent intent) {
        startDM(context);
    }

    // Send a reminder notification
    private void startDM(Context context) {
        Intent emaIntent = new Intent(context, MainActivity.class);
        emaIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(emaIntent);
    }
}