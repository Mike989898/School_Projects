package com.dancemaster.dancemaster;

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.support.design.widget.NavigationView;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Spannable;
import android.text.SpannableString;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.widget.Toast;

import com.dancemaster.dancemaster.Dances.Dance;
import com.dancemaster.dancemaster.Dances.DancesFragment;
import com.dancemaster.dancemaster.FancyUI.GoodDogTextView;
import com.dancemaster.dancemaster.FancyUI.GoodDogTypefaceSpan;
import com.dancemaster.dancemaster.SocialServer.AddFriendActivity;
import com.dancemaster.dancemaster.SocialServer.CreateUsernameActivity;
import com.dancemaster.dancemaster.SocialServer.FriendsFragment;
import com.dancemaster.dancemaster.SocialServer.GcmGetDataMainTask;
import com.dancemaster.dancemaster.SocialServer.GcmRegistrationTask;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    // Instance variables
    public static FragmentManager fragmentManager;
    public static ArrayList<Fragment> fragments;
    public static Integer currentPage;
    public static File dir;
    public static ArrayList<Dance> dances = new ArrayList<Dance>();
    public static HashMap<String, Integer> friendsPoints;
    private static final String CURRENT_PAGE_KEY = "current_page";
    private GetDataBroadcastReceiver broadcastReceiver;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Get the user's past daily scores from the CSV if it exists.
        // Initialize the dailyPoints HashMap
        dir = this.getFilesDir();
        List<String[]> scoreEntries = GLOBALS.readFromCSV(
                new File(dir, "score_entries.csv"));
        if (scoreEntries != null) {
            GLOBALS.dailyPoints = GLOBALS.createHashMap(scoreEntries);
        }

        // Set up the toolbar, drawer, header TextView, and Navigation View
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        GoodDogTextView header = (GoodDogTextView) findViewById(R.id.header);
        header.setTextColor(Color.parseColor("#3161a6"));

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
        changeFontofMenuItems(navigationView);

        // Initialize all the fragments
        fragments = new ArrayList<>();
        fragments.add(new ProgressFragment());
        fragments.add(new DancesFragment());
        fragments.add(new FriendsFragment());
        fragments.add(new SettingsFragment());
        fragments.add(new AboutFragment());

        fragmentManager = getFragmentManager();

        // Find the current page, if one is saved
        if (savedInstanceState != null) {
            currentPage = savedInstanceState.getInt(CURRENT_PAGE_KEY);
        }

        // Otherwise, default to the progress fragment
        if (currentPage == null) {
            currentPage = 0;
        }

        // Set up the view to show the current page
        fragmentManager.beginTransaction()
                .replace(R.id.fragment_view, fragments.get(currentPage)).commit();

        // Initialize the fixed dance information
        dances.add(new Dance("Side Step and Clap", "A simple side to side shuffle!",
                GLOBALS.VIDEO_KEYS[0], GLOBALS.VIDEO_KEYS[1]));
        dances.add(new Dance("Lean and Drop", "Turn, lean, and drop fluidly!",
                GLOBALS.VIDEO_KEYS[2], GLOBALS.VIDEO_KEYS[3]));
        dances.add(new Dance("Pivot", "Spin around in a four part move!",
                GLOBALS.VIDEO_KEYS[4], GLOBALS.VIDEO_KEYS[5]));
        dances.add(new Dance("Bounce", "Alternate feet, dance to the beat!",
                GLOBALS.VIDEO_KEYS[6], GLOBALS.VIDEO_KEYS[7]));
        dances.add(new Dance("Shuffle", "Run in place (with style)!",
                GLOBALS.VIDEO_KEYS[8], GLOBALS.VIDEO_KEYS[9]));
        dances.add(new Dance("Grapevine", "Four steps to the side!",
                GLOBALS.VIDEO_KEYS[10], GLOBALS.VIDEO_KEYS[11]));

        // If no username is saved, prompt the user to create a unique username
        GLOBALS.preferences = getPreferences(Context.MODE_PRIVATE);
        String username = GLOBALS.preferences.getString(getString(R.string.username), "");
        if (GLOBALS.preferences == null || (username).equals("")) {
            Intent intent = new Intent(this, CreateUsernameActivity.class);
            startActivity(intent);
        } else { // Otherwise, connect to the cloud as usual
            new GcmRegistrationTask(this).execute();
        }

        // Initialize and register the broadcast receiver for getting friends data from the server
        broadcastReceiver = new GetDataBroadcastReceiver();
        LocalBroadcastManager lbm = LocalBroadcastManager.getInstance(getApplicationContext());
        lbm.registerReceiver(broadcastReceiver, new IntentFilter(GLOBALS.ACTION_DATA));
    }


    // Save the current page
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putInt(CURRENT_PAGE_KEY, currentPage);
    }


    /** Close the drawer if open. If closed, open it. */
    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        // Is the drawer open?
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            // If so, close the drawer
            drawer.closeDrawer(GravityCompat.START);
        // Otherwise, if the drawer ISN'T Open...
        } else {
            // Open the drawer
            drawer.openDrawer(GravityCompat.START);
        }
    }


    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item selection
        int id = item.getItemId();

        // For each case, update the current page and the view
        switch (id) {
            case R.id.nav_home:
                currentPage = 0;
                fragmentManager
                        .beginTransaction()
                        .replace(R.id.fragment_view, fragments.get(currentPage))
                        .commit();
                break;

            case R.id.nav_dances:
                currentPage = 1;
                fragmentManager
                        .beginTransaction()
                        .replace(R.id.fragment_view, fragments.get(currentPage))
                        .commit();
                break;

            case R.id.nav_friends:
                currentPage = 2;
                fragmentManager
                        .beginTransaction()
                        .replace(R.id.fragment_view, fragments.get(currentPage))
                        .commit();
                break;

            case R.id.nav_settings:
                currentPage = 3;
                fragmentManager
                        .beginTransaction()
                        .replace(R.id.fragment_view, fragments.get(currentPage))
                        .commit();
                break;


            case R.id.nav_about:    // Open the webpage
                currentPage = 4;
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("http://unbouncepages.com/dance-master/"));
                startActivity(browserIntent);
                break;
        }
        // Close the drawer
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }


    // Button click function from the FriendsFragment
    public void onAddFriendsClicked(View v) {
        // Launch the add friends activity
        startActivity(new Intent(this, AddFriendActivity.class));
    }


    /** Following two functions are code taken from stack overflow
     * to change the font of the items in the menu */
    private void applyFontToMenuItem(MenuItem mi) {
        Typeface font = Typeface.createFromAsset(getAssets(), "fonts/GoodDog.TTF");
        SpannableString mNewTitle = new SpannableString(mi.getTitle());
        mNewTitle.setSpan(new GoodDogTypefaceSpan("" , font), 0, mNewTitle.length(),
                Spannable.SPAN_INCLUSIVE_INCLUSIVE);
        mi.setTitle(mNewTitle);
    }

    private void changeFontofMenuItems(NavigationView navView) {
        // Code of the for loop below taken from StackOverflow: to change the font
        Menu m = navView.getMenu();
        for (int i = 0; i < m.size(); i++) {
            MenuItem mi = m.getItem(i);

            // For applying a font to subMenu
            SubMenu subMenu = mi.getSubMenu();
            if (subMenu != null && subMenu.size() > 0) {
                for (int j = 0; j < subMenu.size(); j++) {
                    MenuItem subMenuItem = subMenu.getItem(j);
                    applyFontToMenuItem(subMenuItem);
                }
            }
            applyFontToMenuItem(mi);
        }
    }


    /**
     * A helper class to receive the friends data from the server
     */
    private class GetDataBroadcastReceiver extends BroadcastReceiver {
        // Receive a broadcast with response from service
        @Override
        public void onReceive(Context context, Intent intent) {
            // Get the server's result from the intent
            String result = intent.getStringExtra(GLOBALS.MESSAGE_CODE_DATA);

            // If the result is requesting an update, then start the task to update
            if (result.equals(GLOBALS.MESSAGE_CODE_UPDATE)) {
                try {
                    new GcmGetDataMainTask().execute();
                } catch (Exception e) {
                    Toast.makeText(getApplicationContext(),
                            R.string.ui_toast_connection_unsuccessful,
                            Toast.LENGTH_SHORT).show();
                }
            } else {      // Otherwise, get the data from the server
                friendsPoints = new HashMap<>();    // Initialize the HashMap
                try {       // Parse the string in to the appropriate list
                    JSONArray ja = new JSONArray(result);
                    JSONObject jo;

                    // Get the data from each JSON object and add it to the list
                    for (int i = 0; i < ja.length(); i += 2) {
                        jo = ja.getJSONObject(i);
                        String username = jo.getString("username");
                        jo = ja.getJSONObject(i + 1);
                        Integer points = Integer.parseInt(jo.getString("points"));

                        friendsPoints.put(username, points);
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
