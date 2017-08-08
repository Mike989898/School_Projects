package com.dancemaster.dancemaster.FancyUI;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.EditText;

/**
 * Customize the EditText with a special font, font size, text color,
 * background color, and hint text color.
 */
public class GoodDogEditText extends EditText {
    public GoodDogEditText(Context context, AttributeSet as) {
        super(context, as);
        this.setTypeface(Typeface.createFromAsset(context.getAssets(), "fonts/GoodDog.TTF"));
        this.setTextSize(25);
        this.setTextColor(Color.parseColor("#3333FF"));
        this.getBackground().setColorFilter(Color.parseColor("#3161A6"), PorterDuff.Mode.SRC_IN);
        this.setHintTextColor(Color.parseColor("#B3C6FF"));
    }
}
