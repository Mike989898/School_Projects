package com.dancemaster.dancemaster;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;

/**
 * A way to create dialog fragments. Used in ProfileActivity
 * to take a picture from camera or choose a picture from gallery.
 */
public class DanceDialogFragment extends DialogFragment {
    // Instance variables
    public static final int ID_PHOTO_FROM_CAMERA = 0;
    public static final int ID_PHOTO_FROM_GALLERY = 1;
    public static final int PICK_PHOTO_DID = 1;
    private static final String DIALOG_ID_KEY = "dialog_id";


    /** @param dialog_id: int. the id of this dialog. for the mapping's value
     * @return a new fragment instance
     */
    public static DanceDialogFragment newFrag(int dialog_id) {
        Bundle args = new Bundle();
        args.putInt(DIALOG_ID_KEY, dialog_id);

        DanceDialogFragment frag = new DanceDialogFragment();
        frag.setArguments(args);
        return frag;
    }


    /** @param savedInstanceState: mapping from String to int.
     * @return Dialog. small pop-up window
     */
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        // Which dialog button did the user select?
        final int d_id = getArguments().getInt(DIALOG_ID_KEY);
        final Activity mAct = getActivity();    // Reference to this activity
        AlertDialog.Builder builder = new AlertDialog.Builder(mAct);

        // Create pop up window with title and options and set up a click listener
        if (d_id == PICK_PHOTO_DID) {
            builder.setTitle(R.string.pick_prof_photo);

            DialogInterface.OnClickListener listen = new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int item) {
                    ((ProfileActivity) mAct).onPickPhotoSelected(item);
                }
            };
            builder.setItems(R.array.pick_photo_items, listen);

        }
        return builder.create();    // Return the fruit of this hard work
    }
}
