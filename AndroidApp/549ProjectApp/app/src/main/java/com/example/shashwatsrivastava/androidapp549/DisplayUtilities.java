package com.example.shashwatsrivastava.androidapp549;

import android.content.res.Resources;
import android.util.DisplayMetrics;

/**
 * Created by shashwatsrivastava on 4/5/16.
 */
public class DisplayUtilities {

    /**
     * Utility method to convert density independent pixels to device specific pixels
     */
    public static int dpToPx(float dp) {
        return (int) (dp * Resources.getSystem().getDisplayMetrics().density + 0.5f);
    }

    /**
     * Gets the height of the screen in DP
     * @return Height of screen in dp
     */
    public static float getDpHeight() {
        DisplayMetrics displayMetrics = Resources.getSystem().getDisplayMetrics();
        return displayMetrics.heightPixels / displayMetrics.density;
    }

    /**
     * Gets the width of the screen in DP
     * @return Width of screen in dp
     */
    public static float getDpWidth() {
        DisplayMetrics displayMetrics = Resources.getSystem().getDisplayMetrics();
        return displayMetrics.widthPixels / displayMetrics.density;
    }


}
