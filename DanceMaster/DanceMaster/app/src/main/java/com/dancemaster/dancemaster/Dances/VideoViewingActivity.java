package com.dancemaster.dancemaster.Dances;

import android.content.Context;
import android.content.SharedPreferences;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.os.Bundle;
import android.widget.Toast;

import com.dancemaster.dancemaster.GLOBALS;
import com.dancemaster.dancemaster.R;
import com.dancemaster.dancemaster.SocialServer.ServerMessenger;
import com.google.android.youtube.player.YouTubeBaseActivity;
import com.google.android.youtube.player.YouTubeInitializationResult;
import com.google.android.youtube.player.YouTubePlayer;
import com.google.android.youtube.player.YouTubePlayerView;

import java.io.IOException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

/**
 * VideoViewingActivity is the activity that contains a single video
 * that is played by the user. The video takes up the entire screen.
 */
public class VideoViewingActivity extends YouTubeBaseActivity {
    // Instance variables
    // View that contains the video
    private YouTubePlayerView youTubePlayerView;

    // Listener for the videos
    private YouTubePlayer.OnInitializedListener onInitializedListener;
    private YouTubePlayer.PlayerStateChangeListener onPlayerStateChangedListener;

    String code = "";           // Video to display's code
    Context mContext;           // Context of the activity


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_viewing);
        mContext = this;

        // Get the video code
        code = getIntent().getStringExtra(RecyclerAdapter.CODE);
        youTubePlayerView = (YouTubePlayerView) findViewById(R.id.youtube_view);

        // Implement methods for an OnInitializedListener.
        onInitializedListener = new YouTubePlayer.OnInitializedListener() {
            @Override
            public void onInitializationSuccess(YouTubePlayer.Provider provider,
                                        YouTubePlayer youTubePlayer, boolean b) {
                youTubePlayer.setFullscreen(true);
                youTubePlayer.setFullscreenControlFlags(
                        YouTubePlayer.FULLSCREEN_FLAG_CONTROL_ORIENTATION);
                youTubePlayer.setPlayerStateChangeListener(onPlayerStateChangedListener);
                youTubePlayer.loadVideo(code);
            }

            @Override
            public void onInitializationFailure(YouTubePlayer.Provider provider,
                    YouTubeInitializationResult youTubeInitializationResult) {
            }
        };

        // Implment methods for a player state changed listener
        onPlayerStateChangedListener = new YouTubePlayer.PlayerStateChangeListener() {
            @Override
            public void onLoading() {}

            @Override
            public void onLoaded(String s) {}

            @Override
            public void onAdStarted() {}

            @Override
            public void onVideoStarted() {}

            @Override
            public void onError(YouTubePlayer.ErrorReason errorReason) {}


            // When the user has reached the end of the video, give them points.
            @Override
            public void onVideoEnded() {
                // Create a Date key, truncating the time and
                // replacing the timezone to universal time.
                Date rightNow = new Date();
                Date todayWithoutTime = new Date(rightNow.getYear(), rightNow.getMonth(),
                                                 rightNow.getDate(), 0, 0, 0);
                String key = todayWithoutTime.toString();
                key = key.replaceAll("EDT", "UTC");

                // If the user has already watched a video today
                if (GLOBALS.dailyPoints.get(key) != null) {
                    int CurrScore = GLOBALS.dailyPoints.get(key);
                    CurrScore += GLOBALS.numPointsPerVideo;
                    GLOBALS.dailyPoints.put(key, CurrScore);
                } else {      // Give the user their first 10 points of the day
                    GLOBALS.dailyPoints.put(key, GLOBALS.numPointsPerVideo);
                }

                GLOBALS.writeHashMapToCSV(GLOBALS.dailyPoints);

                try {       // Run an async task to update the points.
                    (new UpdatePointsTask()).execute(key);
                } catch (Exception e) {     // Make a sad toast.
                    Toast.makeText(getApplicationContext(), "Unable to connect to cloud!",
                            Toast.LENGTH_SHORT).show();
                }

                // Update the video watched status to true
                // if this is the first time watching.
                if (!GLOBALS.preferences.getBoolean(code, false)) {
                    SharedPreferences.Editor editor = GLOBALS.preferences.edit();
                    editor.putBoolean(code, true);
                    editor.apply();
                }
            }
        };
    }


    @Override
    public void onStart() {
        super.onStart();
        youTubePlayerView.initialize("AIzaSyC9Xa0NkIc7tpX8bqvQ_JHwCF7sv7kKKWo",
                onInitializedListener);
    }


    // Play notification sound
    private void playSound(int resId){
        MediaPlayer mp = MediaPlayer.create(this, resId);
        mp.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mediaPlayer) {
                mediaPlayer.reset();
                mediaPlayer.release();
            }
        });
        mp.start();
    }


    /**
     * Upload the points update to the server
     */
    private class UpdatePointsTask extends AsyncTask<String, Void, Void> {
        // Send a message to the server with this phone's registration,
        // the new points and the date
        @Override
        protected Void doInBackground(String... key) {
            try {
                // Create the map to send to the server
                Map<String, String> map = new HashMap<>();
                map.put(GLOBALS.MESSAGE_KEY_REGID, GLOBALS.regID);
                map.put(GLOBALS.MESSAGE_KEY_DATE, key[0]);
                map.put(GLOBALS.MESSAGE_KEY_POINTS,
                        Integer.toString(GLOBALS.dailyPoints.get(key[0])));

                // Send the message to the server
                ServerMessenger.send(GLOBALS.SERVER_ADDR + "/addPoints.do", map);
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }
}
