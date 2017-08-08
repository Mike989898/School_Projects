package com.dancemaster.dancemaster.Dances;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.dancemaster.dancemaster.GLOBALS;
import com.dancemaster.dancemaster.MainActivity;
import com.dancemaster.dancemaster.R;

import java.util.ArrayList;

/**
 * DanceListAdapter is a custom adapter for the items of the dances list.
 */
public class DanceListAdapter extends ArrayAdapter<Dance> {
    private LayoutInflater inflater;

    // Constructor to save the context and set the inflater
    public DanceListAdapter(Context context) {
        super(context, R.layout.dance_list_item);
        inflater = LayoutInflater.from(context);
    }

    // Reset the data to reflect any changes.
    public void setData(ArrayList<Dance> dances) {
        clear();
        if (dances != null) {
            addAll(dances);
        }
    }

    // Define the custom view to be returned for each item in the list
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // Get the dance object from the list
        Dance dance = MainActivity.dances.get(position);

        // Create a new view from the template if none is being recycled
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.dance_list_item, parent, false);
        }

        // Get the text views and image view from the template view
        ImageView thumbnail = (ImageView) convertView.findViewById(R.id.thumbnail);
        TextView header = (TextView) convertView.findViewById(R.id.dance_title);
        TextView description = (TextView) convertView.findViewById(R.id.dance_description);
        ImageView lock = (ImageView) convertView.findViewById(R.id.lock_icon);

        // Fill the views with information from the dance object
        header.setText(dance.getTitle());
        description.setText(dance.getDescription());

        // Set the thumbnail pictures for each dance
        switch (position) {
            case 0:
                thumbnail.setImageResource(R.drawable.dance1_thumbnail);
                break;
            case 1:
                thumbnail.setImageResource(R.drawable.dance2_thumbnail);
                break;
            case 2:
                thumbnail.setImageResource(R.drawable.dance3_thumbnail);
                break;
            case 3:
                thumbnail.setImageResource(R.drawable.dance4_thumbnail);
                break;
            case 4:
                thumbnail.setImageResource(R.drawable.dance5_thumbnail);
                break;
            case 5:
                thumbnail.setImageResource(R.drawable.dance6_thumbnail);
                break;
        }

        // The first video is always unlocked
        if (position == 0) {
            lock.setImageResource(R.drawable.unlocked);
            convertView.setClickable(false);
            convertView.setFocusable(false);
        } else {  // Assume this video is unlocked
            boolean isLocked = false;

            // For each previous video...
            for (int i = 0; i < (position * 2); i++) {
                // If we encounter an unwatched video...
                if (!GLOBALS.preferences.getBoolean(GLOBALS.VIDEO_KEYS[i], false)) {
                    // This video is LOCKED!
                    isLocked = true;
                    break;
                }
            }

            // Set the appropriate image
            if (isLocked) {
                lock.setImageResource(R.drawable.locked);
                convertView.setClickable(true);
                convertView.setFocusable(true);
            } else {
                lock.setImageResource(R.drawable.unlocked);
                convertView.setClickable(false);
                convertView.setFocusable(false);
            }
        }
        // Return the completed view
        return convertView;
    }
}
