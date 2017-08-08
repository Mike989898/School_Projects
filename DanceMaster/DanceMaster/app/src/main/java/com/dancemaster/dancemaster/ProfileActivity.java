package com.dancemaster.dancemaster;

import android.Manifest;
import android.app.Activity;
import android.app.DialogFragment;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.Toast;

import com.dancemaster.dancemaster.FancyUI.CircleImageView;
import com.soundcloud.android.crop.Crop;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Allows the user to enter his/her user profile information.
 */
public class ProfileActivity extends Activity {
    // Instance variables
    private static final int PERMISSIONS = 0;
    public static final int TAKE_FROM_CAMERA = 0;
    public static final int TAKE_FROM_GALLERY = 1;

    private boolean isTakenFromCamera;
    private ImageView mImageView;
    private Uri mImageCapUri;
    private Uri tmpImageCapUri;
    private static final String URI_INSTANCE_KEY = "saved_uri";
    private SharedPreferences prefs = GLOBALS.preferences;


    /** Called when the app is first opened or when the orientation is rotated
     * @param savedInstanceState: Bundle. Info needed to create the app. */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        mImageView = (ImageView) findViewById(R.id.imageProfile);
        loadProfPic();
        if (prefs != null) {
            loadProfile();     // Load the saved profile info
        }

        if (savedInstanceState != null) {   // Called when the screen is rotated
            mImageCapUri = savedInstanceState.getParcelable(URI_INSTANCE_KEY);
            if (mImageCapUri != null) {
                mImageView.setImageURI(mImageCapUri);
            }
        }
    }



    /**
     * Save the image uri before the activity is destroyed
     * @param outState the Bundle that holds the information
     */
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putParcelable(URI_INSTANCE_KEY, mImageCapUri);
    }


    /**
     * Consulted Activity Lifecycle Lecture Notes.
     * Save the user-inputted information: name, phone, email, class, major,
     * gender, and profile picture in a Preferences editor.
     */
    public void saveProfile() {
        saveProfPic();     // Save the photo.

        prefs = GLOBALS.preferences;
        SharedPreferences.Editor editor = prefs.edit();

        editor.putString(getString(R.string.name),
                ((EditText) findViewById(R.id.name)).getText().toString());
        editor.putString(getString(R.string.email),
                ((EditText) findViewById(R.id.email)).getText().toString());
        editor.putInt(getString(R.string.gender),
                ((RadioGroup) findViewById(R.id.gender)).getCheckedRadioButtonId());
        editor.apply();  // Commit changes into shared preference
    }


    /** Retrieve the information that the user has entered in the profile:
     * profile picture, name, phone number, email, class, gender, and major. */
    public void loadProfile() {
        String name = prefs.getString(getString(R.string.name), "");
        if (!name.equals("")) {
            ((EditText) findViewById(R.id.name)).setText(name);
        }

        String phone = prefs.getString(getString(R.string.username), "");
        if (!phone.equals("")) {
            ((EditText) findViewById(R.id.username)).setText(phone);
        }

        String email = prefs.getString(getString(R.string.email), "");
        if (!email.equals("")) {
            ((EditText) findViewById(R.id.email)).setText(email);
        }

        int gender = prefs.getInt(getString(R.string.gender), -1);
        if (gender != -1) {
            ((RadioGroup) findViewById(R.id.gender)).check(gender);
        }
    }


    /* Button methods */
    public void onSaveClicked(View v) {
        saveProfile();  // Save photo and entered profile information

        // Tell the user the profile has been saved.
        Toast.makeText(getApplicationContext(),
                getString(R.string.profile_toast_save),
                Toast.LENGTH_SHORT).show();
        finish();   // Close the activity
    }


    // Close the activity
    public void onCancelClicked(View v) {
        finish();
    }


    /**
     * Change the profile photo via popping up a dialog that
     * asks the user to take a picture.
     */
    public void onChangePhotoClicked(View v) {
        checkPermissions();
        displayDialog(DanceDialogFragment.PICK_PHOTO_DID);
    }


    /**
     * Method that is called in MyRunsDialogFragment.
     * Handles the photo from camera or gallery.
     *
     * @param item: int. the position of the item that was selected in the dialog.
     */
    public void onPickPhotoSelected(int item) {
        Intent intent;

        switch (item) {
            case DanceDialogFragment.ID_PHOTO_FROM_CAMERA:
                intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                mImageCapUri = Uri.fromFile(new File(Environment
                        .getExternalStorageDirectory(), "tmp_"
                        + String.valueOf(System.currentTimeMillis()) + ".jpg"));
                intent.putExtra(MediaStore.EXTRA_OUTPUT, mImageCapUri);
                intent.putExtra("return-data", true);
                try {  // Try starting camera
                    startActivityForResult(intent, TAKE_FROM_CAMERA);
                } catch (ActivityNotFoundException e) {
                    e.printStackTrace();
                }
                isTakenFromCamera = true;
                break;

            case DanceDialogFragment.ID_PHOTO_FROM_GALLERY:
                intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                intent.setType("image/*");
                intent.setAction(Intent.ACTION_GET_CONTENT);
                startActivityForResult(intent, TAKE_FROM_GALLERY);
                isTakenFromCamera = false;
                break;
        }
    }


    /* Display the dialog fragment by creating an instance of DialogFragment */
    public void displayDialog(int id) {
        DialogFragment fragment = DanceDialogFragment.newFrag(id);
        fragment.show(getFragmentManager(), "dialog_fragment_photo_picker");
    }


    private void checkPermissions() {
        // Consulted developer.android.com/training/permissions/requesting.html
        // Check the camera permissions
        int permissionCheckCamera = ContextCompat.checkSelfPermission(
                this, android.Manifest.permission.CAMERA);

        // Check the storage permissions- to show the prof pic
        int permissionCheckStorage = ContextCompat.checkSelfPermission(
                this, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        // PERMISSION_DENIED: explicitly ask user for camera or storage permission
        if (permissionCheckCamera != PackageManager.PERMISSION_GRANTED ||
                permissionCheckStorage != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{android.Manifest.permission.CAMERA,
                            Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    PERMISSIONS);
        }
    }


    /**
     * Because requiring access to the camera is *dangerous*,
     * we need to explicitly ask the user for permission.
     * Consulted developer.android.com/training/permissions/requesting.html
     */
    @Override
    public void onRequestPermissionsResult(int requestCode,
                       String permissions[], int[] grantResults) {
        switch (requestCode) {
            case PERMISSIONS:
                // permission was granted
                if (grantResults.length > 0 &&
                        grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    // permission for camera and storage must be granted!!!
                    Toast.makeText(ProfileActivity.this, "Need to allow permissions",
                            Toast.LENGTH_SHORT).show();
                    finish();       // exit the app
                }
        }
    }


    /**
     * Called when the person has approved a photo taken from camera
     * or approved a photo to be cropped.
     * @param requestCode: int. what did the user want to do?
     * @param resultCode:  int. is there a problem yet?
     * @param data:        Intent. message that is sent here to crop
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            switch (requestCode) {
                // if from camera or gallery, send image for cropping
                case TAKE_FROM_CAMERA:
                    beginCrop(mImageCapUri);
                    break;

                case TAKE_FROM_GALLERY:
                    mImageCapUri = data.getData();
                    beginCrop(mImageCapUri);
                    break;

                case Crop.REQUEST_CROP:
                    // Update image view after image crop
                    handleCrop(resultCode, data);

                    // Delete temp photo taken by camera that was uncropped.
                    if (isTakenFromCamera) {
                        File f = new File(mImageCapUri.getPath());
                        if (f.exists())
                            f.delete();
                    }
                    mImageCapUri = tmpImageCapUri;
                    break;
            }
        }
    }


    /* Load profile photo from internal storage.
    This is a reason the app needs to access storage. */
    private void loadProfPic() {
        try {
            FileInputStream fis = openFileInput(
                    getString(R.string.prof_photo_filename));
            Bitmap bmap = BitmapFactory.decodeStream(fis);
            mImageView.setImageBitmap(bmap);

            fis.close();
        } catch (IOException e) {
            // Default profile photo if no photo saved before.
            mImageView.setImageResource(R.drawable.smile);
            ((CircleImageView) mImageView).setBorderWidth(0);
        }
    }


    /* Save the new profile picture in internal storage.
    This is a reason the app needs to access storage. */
    private void saveProfPic() {
        mImageView.buildDrawingCache();
        Bitmap bmap = mImageView.getDrawingCache();
        try {
            FileOutputStream fos = openFileOutput(
                    getString(R.string.prof_photo_filename), MODE_PRIVATE);
            bmap.compress(Bitmap.CompressFormat.PNG, 100, fos);
            fos.flush();
            fos.close();
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }


    /**
     * Consulted crop methods discussed in class.
     **/
    private void beginCrop(Uri source) {
        Uri destination = Uri.fromFile(new File(getCacheDir(), "cropped"));
        Crop.of(source, destination).asSquare().start(this);
    }


    private void handleCrop(int resultCode, Intent result) {
        if (resultCode == RESULT_OK) {
            mImageView.setImageResource(0);  // reset the Image Resource
            mImageView.setImageURI(Crop.getOutput(result));

            // tmpImageCapUri used to keep the pic after rotations
            tmpImageCapUri = Crop.getOutput(result);
        } else if (resultCode == Crop.RESULT_ERROR) {
            Toast.makeText(this, Crop.getError(result).getMessage(),
                    Toast.LENGTH_SHORT).show();
        }
    }
}