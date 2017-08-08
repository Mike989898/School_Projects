package com.dancemaster.dancemaster.Dances;

import android.app.ListFragment;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import com.dancemaster.dancemaster.MainActivity;

/**
 * DancesFragment is a ListFragment that contains all the dances in a list.
 */
public class DancesFragment extends ListFragment {
    // Instance variables
    public ListView listView;
    public DanceListAdapter adapter;
    private Context context;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        setRetainInstance(true);
        context = getActivity().getApplicationContext();

        // Initialize the custom adapter if necessary
        if (adapter == null) {
            adapter = new DanceListAdapter(context);
            adapter.setData(MainActivity.dances);
        }

        // Attach the adapter to the list view
        listView = new ListView(context);
        listView.setAdapter(adapter);

        return listView;
    }


    /* When a Dance item is clicked, open the DanceVideoActivity */
    @Override
    public void onListItemClick(ListView listView, View view, int position, long id) {
        Intent intent = new Intent(context, DanceVideoActivity.class);
        intent.putExtra(DanceVideoActivity.INDEX, position);
        startActivity(intent);
    }


    /** Update the view when the fragment resumes
     * so that the lock icons will update accordingly */
    @Override
    public void onResume() {
        super.onResume();

        // Update the lock icons in the Dances Fragment
        adapter.clear();
        adapter.setData(MainActivity.dances);

        // Attach the adapter to the list view
        listView = new ListView(context);
        listView.setAdapter(adapter);
    }
}
