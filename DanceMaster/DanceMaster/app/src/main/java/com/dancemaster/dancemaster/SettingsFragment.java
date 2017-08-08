package com.dancemaster.dancemaster;

import android.app.TimePickerDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.view.View;
import android.widget.ListView;
import android.widget.TimePicker;

import com.dancemaster.dancemaster.Reminder.DMScheduler;

import java.util.Calendar;

/**
 * Allows the user to set a reminder time, which will go off
 * if the user chooses to be reminded.
 * Created by Kristy and Nick on 5/26/2016.
 */
public class SettingsFragment extends PreferenceFragment {
    private Calendar myDateAndTime = Calendar.getInstance();
    private boolean isAlarmSet;
    private SharedPreferences prefs;
    View v;

    public static final String HOUR = "hour";
    public static final String MIN = "minute";
    public static final String SEC = "second";


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings_fragment);

        // Set the reminder
        CheckBoxPreference alarm = (CheckBoxPreference) findPreference(
                getString(R.string.reminder_option_key));
        isAlarmSet = alarm.isChecked();

        prefs = getActivity().getPreferences(Context.MODE_PRIVATE);
        if (prefs != null) {
            loadMyDateAndTime();
        }

        Preference button = findPreference(getString(R.string.reminder_time_key));
        button.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
            @Override
            public boolean onPreferenceClick(Preference preference) {

                // Set the reminder time
                TimePickerDialog.OnTimeSetListener myTimeListener = new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker timePicker, int hourOfDay, int minute) {
                        DMScheduler.cancel(getActivity(), myDateAndTime);

                        myDateAndTime.set(Calendar.HOUR_OF_DAY, hourOfDay);
                        myDateAndTime.set(Calendar.MINUTE, minute);
                        myDateAndTime.set(Calendar.SECOND, 0);

                        saveMyDateAndTime(hourOfDay, minute);

                        if (isAlarmSet) {
                            DMScheduler.setSchedule(getActivity(), myDateAndTime);
                        }
                    }
                };

                new TimePickerDialog(getActivity(), myTimeListener,
                        myDateAndTime.get(Calendar.HOUR_OF_DAY),
                        myDateAndTime.get(Calendar.MINUTE), true).show();

                return true;
            }
        });

        Preference checkbox = findPreference(getString(R.string.reminder_option_key));
        checkbox.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object o) {
                isAlarmSet = !isAlarmSet;

                if (isAlarmSet) {
                    DMScheduler.setSchedule(getActivity(), myDateAndTime);
                } else {
                    DMScheduler.cancel(getActivity(), myDateAndTime);
                }

                return true;
            }
        });
    }


    private void loadMyDateAndTime() {
        int hr = prefs.getInt(HOUR, -1);
        if (hr != -1) {
            myDateAndTime.set(Calendar.HOUR_OF_DAY, hr);
        }

        int min = prefs.getInt(MIN, -1);
        if (min != -1) {
            myDateAndTime.set(Calendar.MINUTE, min);
        }

        int sec = prefs.getInt(SEC, -1);
        if (sec != -1) {
            myDateAndTime.set(Calendar.SECOND, sec);
        }
    }


    public void saveMyDateAndTime(int hr, int min) {
        SharedPreferences.Editor editor = prefs.edit();
        editor.putInt(HOUR, hr);
        editor.putInt(MIN, min);
        editor.putInt(SEC, 0);
        editor.apply();
    }


    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        // remove dividers
        View rootView = getView();
        ListView list = (ListView) rootView.findViewById(android.R.id.list);
        list.setDivider(null);

    }
}
