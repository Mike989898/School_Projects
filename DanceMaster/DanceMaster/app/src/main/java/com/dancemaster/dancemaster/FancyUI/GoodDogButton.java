package com.dancemaster.dancemaster.FancyUI;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.Button;

/**
 * Customize the buttons with GoodDog font,
 * large font size, special colors and a special shape.
 */
public class GoodDogButton extends Button {
    public GoodDogButton(Context context, AttributeSet as) {
        super(context, as);
        this.setTypeface(Typeface.createFromAsset(context.getAssets(), "fonts/GoodDog.TTF"));
        this.setTextSize(25);
        this.setTextColor(Color.parseColor("#008000"));
    }
}
