package com.dancemaster.dancemaster.Reminder;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import java.util.Calendar;


/**
 * Created by varun on 1/20/16.
 * Taken from StressMeter.
 */
public class DMScheduler {
    // Sets an alarm for the given time.
    public static void setSchedule(Context context, Calendar time) {
        setSchedule(context, time.get(Calendar.HOUR_OF_DAY),
                time.get(Calendar.MINUTE), time.get(Calendar.SECOND));
    }

    private static void setSchedule(Context context, int hour, int min, int sec) {
        // The request code distinguish different stress meter schedule instances
        int requestCode = hour * 10000 + min * 100 + sec;

        // Set pending intent to call EMAAlarmReceiver.
        Intent intent = new Intent(context, EMAAlarmReceiver.class);
        PendingIntent pi = PendingIntent.getBroadcast(context, requestCode, intent,
                PendingIntent.FLAG_CANCEL_CURRENT);

        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.HOUR_OF_DAY, hour);
        calendar.set(Calendar.MINUTE, min);
        calendar.set(Calendar.SECOND, sec);

        if (calendar.getTimeInMillis() < System.currentTimeMillis()) {
            calendar.add(Calendar.DATE, 1);
        }

        // Set repeating alarm, and pass the pending intent, so that the
        // broadcast is sent everytime the alarm is triggered
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        alarmManager.setRepeating(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(),
                AlarmManager.INTERVAL_DAY, pi);
    }

    public static void cancel(Context context, Calendar time) {
        int requestCode = time.get(Calendar.HOUR_OF_DAY) * 10000 +
                          time.get(Calendar.MINUTE) * 100 +
                          time.get(Calendar.SECOND);

        Intent intent = new Intent(context, EMAAlarmReceiver.class);
        PendingIntent pi = PendingIntent.getBroadcast(context, requestCode, intent,
                PendingIntent.FLAG_CANCEL_CURRENT);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        alarmManager.cancel(pi);
    }
}
