package com.dancemaster.dancemaster.FancyUI;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.RadioButton;

/**
 * Customize the radio button with a special font, text size and color.
 */
public class GoodDogRadioButton extends RadioButton {
    public GoodDogRadioButton(Context context, AttributeSet as) {
        super(context, as);
        this.setTypeface(Typeface.createFromAsset(context.getAssets(), "fonts/GoodDog.TTF"));
        this.setTextSize(25);
        this.setTextColor(Color.parseColor("#3333FF"));
    }
}
