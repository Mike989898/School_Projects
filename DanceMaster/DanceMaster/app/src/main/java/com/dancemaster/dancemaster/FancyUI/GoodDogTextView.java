package com.dancemaster.dancemaster.FancyUI;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.TextView;

/**
 * Customize the Text View with a special font and magenta color.
 */
public class GoodDogTextView extends TextView {
    public GoodDogTextView(Context context, AttributeSet as) {
        super(context, as);
        this.setTypeface(Typeface.createFromAsset(context.getAssets(), "fonts/GoodDog.TTF"));
        this.setTextColor(Color.MAGENTA);
    }
}
