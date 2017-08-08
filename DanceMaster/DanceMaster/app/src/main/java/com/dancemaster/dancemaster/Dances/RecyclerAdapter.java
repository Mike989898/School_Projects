package com.dancemaster.dancemaster.Dances;

import android.content.Context;
import android.content.Intent;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.dancemaster.dancemaster.R;
import com.google.android.youtube.player.YouTubeInitializationResult;
import com.google.android.youtube.player.YouTubeThumbnailLoader;
import com.google.android.youtube.player.YouTubeThumbnailView;

/**
 * RecyclerAdapter is an adapter to hold the videos of each dance.
 *
 */
public class RecyclerAdapter extends
                RecyclerView.Adapter<RecyclerAdapter.VideoInfoHolder> {
    // Instance variables
    public String[] VideoID = new String[2];    // IDs are unique for each video
    private Context ctx;
    protected TextView title;
    private View itemView;      // View container for the item
    public static final String CODE = "code";


    /** Constructor:
     * @param context: Context
     * @param key1: Video key for the instructional video
     * @param key2: Video key for the regular practice video */
    public RecyclerAdapter(Context context, String key1, String key2) {
        this.ctx = context;
        VideoID[0] = key1;
        VideoID[1] = key2;
    }


    @Override
    public VideoInfoHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        itemView = LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.list_item, parent, false);
        return new VideoInfoHolder(itemView);
    }


    @Override
    public void onBindViewHolder(final VideoInfoHolder holder, final int position) {
        title = (TextView) itemView.findViewById(R.id.textView);
        if (position == 1) {
            title.setText(R.string.dance_video);
        }

        final YouTubeThumbnailLoader.OnThumbnailLoadedListener onThumbnailLoadedListener
                = new YouTubeThumbnailLoader.OnThumbnailLoadedListener(){
            @Override
            public void onThumbnailError(YouTubeThumbnailView youTubeThumbnailView,
                                         YouTubeThumbnailLoader.ErrorReason errorReason) {

            }


            @Override
            public void onThumbnailLoaded(YouTubeThumbnailView youTubeThumbnailView, String s) {
                youTubeThumbnailView.setVisibility(View.VISIBLE);
                holder.relativeLayoutOverYouTubeThumbnailView.setVisibility(View.VISIBLE);
            }
        };

        holder.youTubeThumbnailView.initialize("AIzaSyC9Xa0NkIc7tpX8bqvQ_JHwCF7sv7kKKWo",
                new YouTubeThumbnailView.OnInitializedListener() {
            @Override
            public void onInitializationSuccess(YouTubeThumbnailView youTubeThumbnailView,
                                                YouTubeThumbnailLoader youTubeThumbnailLoader) {
                youTubeThumbnailLoader.setVideo(VideoID[position]);
                youTubeThumbnailLoader.setOnThumbnailLoadedListener(onThumbnailLoadedListener);
            }


            @Override
            public void onInitializationFailure(YouTubeThumbnailView youTubeThumbnailView,
                            YouTubeInitializationResult youTubeInitializationResult) {
            }
        });
    }


    @Override
    public int getItemCount() {
        return VideoID.length;
    }


    public class VideoInfoHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        // Instance variables
        protected RelativeLayout relativeLayoutOverYouTubeThumbnailView;
        private YouTubeThumbnailView youTubeThumbnailView;
        protected ImageView playButton;


        // Constructor
        public VideoInfoHolder(View itemView) {
            super(itemView);
            playButton = (ImageView) itemView.findViewById(R.id.btnYoutube_player);
            playButton.setOnClickListener(this);
            relativeLayoutOverYouTubeThumbnailView = (RelativeLayout) itemView
                    .findViewById(R.id.relativeLayout_over_youtube_thumbnail);
            youTubeThumbnailView = (YouTubeThumbnailView)
                    itemView.findViewById(R.id.youtube_thumbnail);
        }


        @Override
        public void onClick(View v) {
            Intent intent = new Intent(ctx, VideoViewingActivity.class);
            intent.putExtra(CODE, VideoID[getLayoutPosition()]);
            ctx.startActivity(intent);
        }
    }
}