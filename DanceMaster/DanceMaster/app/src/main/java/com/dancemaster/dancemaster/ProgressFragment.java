package com.dancemaster.dancemaster;

import android.app.Fragment;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.dancemaster.dancemaster.FancyUI.CircleImageView;
import com.dancemaster.dancemaster.FancyUI.GoodDogTextView;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import lecho.lib.hellocharts.model.Axis;
import lecho.lib.hellocharts.model.AxisValue;
import lecho.lib.hellocharts.model.Line;
import lecho.lib.hellocharts.model.LineChartData;
import lecho.lib.hellocharts.model.PointValue;
import lecho.lib.hellocharts.view.LineChartView;

/**
 * Shows a graph of the user's progress over time.
 */
public class ProgressFragment extends Fragment {
    // Instance variable: the highest possible score
    private static final int MAX_SCORE = 60;
    private List<String[]> CSVEntries;

    // Constants for sizing the profile picture
    public static final int SMILEY_SIZE = 500;


    // Create the Fragment
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Read the time stamps and stress levels from the csv file
        CSVEntries = GLOBALS.convertHashMapToGraphData(GLOBALS.dailyPoints);

        // Inflate the view and create the visuals: line and table chart
        View v = inflater.inflate(R.layout.progress_fragment,
                container, false);

        // Only create the charts if csv file exists
        if (CSVEntries != null) {
            createLineChart(v);
        }

        // Load the profile image as a CircleImageView
        CircleImageView profileImage = (CircleImageView) v.findViewById(R.id.profile_image);
        loadProfPic(profileImage);

        return v;
    }


    private void loadProfPic(CircleImageView mImageView) {
        // These LayoutParams variables will be used to programatically size the pictures
        // because the pictures from the camera / gallery are a different size than the
        // default smiley face picture
        ViewGroup.LayoutParams lp = mImageView.getLayoutParams();

        try {
            // If the user has actually taken a picture, then we will load that picture
            FileInputStream fis = getActivity().openFileInput(
                    getString(R.string.prof_photo_filename));
            Bitmap bmap = BitmapFactory.decodeStream(fis);
            mImageView.setImageBitmap(bmap);
            fis.close();
        } catch (IOException e) {
            // Set the default smiley image
            mImageView.setImageResource(R.drawable.smile);
            mImageView.setBorderWidth(0);
        }

        lp.height = SMILEY_SIZE;
        lp.width = SMILEY_SIZE;
    }


    /**
     * Dynamically updates the data associated with the LineChartView.
     * Draws the line with axes
     */
    public void createLineChart(View v) {
        // Get the points that will be displayed on the line chart
        List<PointValue> values = getLineValues();

        // If the person has not watched any videos in the past seven days...
        boolean noPointsLastSevenDays = true;
        for (int i = 0; i < values.size(); i++) {
            if (values.get(i).getY() != 0) {
                noPointsLastSevenDays = false;
                break;
            }
        }

        if (noPointsLastSevenDays) {
            // Get rid of the LineChartView
            LineChartView lineChart = (LineChartView) v.findViewById(R.id.line_chart);
            lineChart.setVisibility(View.GONE);

            // Set a TextView that disapproves.
            GoodDogTextView noProg = (GoodDogTextView) v.findViewById(R.id.no_progress);
            noProg.setText(getString(R.string.no_progress_message));
            noProg.setTextSize(40);
            noProg.setTextColor(Color.parseColor("#009900"));

        } else {        // Otherwise, draw the graph as usual
            // Set the line color and shape
            List<Line> lines = new ArrayList<>();
            lines.add(new Line(values).setColor(Color.CYAN).setCubic(false));

            // Set the axes on the line chart display
            Axis x = getXAxis(values.size());
            Axis y = getYAxis();

            // Associate the line and axes with the data
            LineChartData data = new LineChartData();
            data.setLines(lines);
            data.setAxisXBottom(x);
            data.setAxisYLeft(y);

            // Update the line chart display
            LineChartView lineChart = (LineChartView) v.findViewById(R.id.line_chart);
            lineChart.setLineChartData(data);
            lineChart.setVisibility(View.VISIBLE);
        }
    }


    /*
     * @param max: total number of days
     * @return Axis object that will be used for the X Axis
     */
    public Axis getXAxis(int max) {
        List<AxisValue> values = new ArrayList<>();

        // Create the list of values on the X Axis
        // A list of floats for 7 days
        for (int i = 0; i <= max; i++) {
            values.add(new AxisValue((float) i));
        }

        // Create the Axis object and give it a name
        Axis x = new Axis(values);
        x.setName("Last 7 Days!");

        x.setTextColor(Color.parseColor("#6600CC"));
        x.setTypeface(Typeface.createFromAsset(getActivity().getAssets(), "fonts/GoodDog.TTF"));
        x.setTextSize(20);

        return x;
    }


    /**
     * @return Axis object that wil be used for the Y Axis
     */
    public Axis getYAxis() {
        List<AxisValue> values = new ArrayList<>();

        // Create the list of stress values on the Y Axis
        // A list of floats from 1 to MAX_STRESS, the possible stress levels
        for (int i = 0; i <= MAX_SCORE; i = i + 10) {
            values.add(new AxisValue((float) i));
        }

        // Create the Axis object and give it a name
        Axis y = new Axis(values);
        y.setName("Score for Each Day!");

        y.setTextColor(Color.parseColor("#6600CC"));
        y.setTypeface(Typeface.createFromAsset(getActivity().getAssets(), "fonts/GoodDog.TTF"));
        y.setTextSize(20);

        return y;
    }

    /**
     * @return lineValues: an ArrayList<PointValue>.
     * Each element in lineValues is an ordered pair of instance and score
     * */
    private List<PointValue> getLineValues() {
        // PointValue takes two floats
        float instance;
        float score;
        ArrayList<PointValue> lineValues = new ArrayList<>();

        // Add each entry from the csv file to an ArrayList<PointValue>
        for (int i = CSVEntries.size() - 7; i < CSVEntries.size(); i++) {
            String[] entry = CSVEntries.get(i);
            instance = (float) i;
            score = Float.parseFloat(entry[1]);
            lineValues.add(new PointValue(instance, score));
        }
        return lineValues;
    }
}