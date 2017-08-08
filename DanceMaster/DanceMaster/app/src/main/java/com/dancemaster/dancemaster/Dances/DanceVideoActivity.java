package com.dancemaster.dancemaster.Dances;

import android.app.Activity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;

import com.dancemaster.dancemaster.MainActivity;
import com.dancemaster.dancemaster.R;

/**
 * Created by Mike on 5/19/2016.
 */

public class DanceVideoActivity extends Activity{
    public static final String INDEX = "index";
    RecyclerAdapter adapter;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        int i = getIntent().getIntExtra(INDEX, -1);
        Dance dance = MainActivity.dances.get(i);

        setContentView(R.layout.activity_dance_video);
        RecyclerView recyclerView = (RecyclerView) findViewById(R.id.list);
        recyclerView.setHasFixedSize(true);

        // Layout manager for recycler view
        LinearLayoutManager linearLayoutManager = new LinearLayoutManager(this);
        linearLayoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        linearLayoutManager.offsetChildrenVertical(2);
        recyclerView.setLayoutManager(linearLayoutManager);
        adapter = new RecyclerAdapter(DanceVideoActivity.this, dance.getVideoCodeTrain(), dance.getVideoCode());
        recyclerView.setAdapter(adapter);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }
}
