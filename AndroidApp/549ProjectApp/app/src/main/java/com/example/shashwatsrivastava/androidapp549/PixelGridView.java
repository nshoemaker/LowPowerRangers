package com.example.shashwatsrivastava.androidapp549;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.widget.RelativeLayout;

import java.util.zip.DeflaterInputStream;

/**
 * Created by shashwatsrivastava on 4/21/16.
 */
public class PixelGridView extends RelativeLayout {
    private int numRows = 30;
    private int cellHeight = (int)DisplayUtilities.getDpHeight() / numRows;
    private int cellWidth = cellHeight;
    private int numColumns = (int) DisplayUtilities.getDpWidth() / cellWidth;
    private Paint redPaint = new Paint();
    // Will be used to take care of scaling
    private ScaleGestureDetector mScaleDetector;
    private float mScaleFactor = 1.f;

    //These constants specify the mode that we're in
    private static int NONE = 0;
    private static int DRAG = 1;
    private static int ZOOM = 2;

    private int mode;

    //These two variables keep track of the X and Y coordinate of the finger when it first
    //touches the screen
    private float startX = 0f;
    private float startY = 0f;

    //These two variables keep track of the amount we need to translate the canvas along the X
    //and the Y coordinate
    private float translateX = 0f;
    private float translateY = 0f;

    //These two variables keep track of the amount we translated the X and Y coordinates, the last time we
    //panned.
    private float previousTranslateX = 0f;
    private float previousTranslateY = 0f;

    public PixelGridView(Context context) {
        this(context, null);
        redPaint.setColor(Color.RED);
        setWillNotDraw(false);
        mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
    }

    public PixelGridView(Context context, AttributeSet attrs) {
        super(context, attrs);
        redPaint.setStyle(Paint.Style.FILL_AND_STROKE);
        redPaint.setColor(Color.RED);
        setWillNotDraw(false);
        mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
    }

    public void setNumColumns(int numColumns) {
        this.numColumns = numColumns;
        calculateDimensions();
    }

    public int getNumColumns() {
        return numColumns;
    }

    public int getCellHeight() { return this.cellHeight; }

    public int getCellWidth() { return this.cellWidth; }

    public void setNumRows(int numRows) {
        this.numRows = numRows;
        calculateDimensions();
    }

    public int getNumRows() {
        return numRows;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        calculateDimensions();
    }

    private void calculateDimensions() {
        if (numColumns < 1 || numRows < 1) {
            return;
        }

        cellWidth = getWidth() / numColumns;
        cellHeight = getHeight() / numRows;

        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {


        canvas.save();
        canvas.scale(mScaleFactor, mScaleFactor);
        super.onDraw(canvas);
        float displayWidth = DisplayUtilities.getDpWidth();
        float displayHeight = DisplayUtilities.getDpHeight();

        //If translateX times -1 is lesser than zero, let's set it to zero. This takes care of the left bound
        if((translateX * -1) < 0) {
            translateX = 0;
        }
        //This is where we take care of the right bound. We compare translateX times -1 to (scaleFactor - 1) * displayWidth.
        //If translateX is greater than that value, then we know that we've gone over the bound. So we set the value of
        //translateX to (1 - scaleFactor) times the display width. Notice that the terms are interchanged; it's the same
        //as doing -1 * (scaleFactor - 1) * displayWidth
        else if((translateX * -1) > (mScaleFactor - 1) * displayWidth) {
            translateX = (1 - mScaleFactor) * displayWidth;
        }

        if(translateY * -1 < 0) {
            translateY = 0;
        }

        //We do the exact same thing for the bottom bound, except in this case we use the height of the display
        else if((translateY * -1) > (mScaleFactor - 1) * displayHeight) {
            translateY = (1 - mScaleFactor) * displayHeight;
        }

        //We need to divide by the scale factor here, otherwise we end up with excessive panning based on our zoom level
        //because the translation amount also gets scaled according to how much we've zoomed into the canvas.
        canvas.translate(translateX / mScaleFactor, translateY / mScaleFactor);

        canvas.drawColor(Color.BLACK);

        if (numColumns == 0 || numRows == 0) {
            return;
        }

        int width = getWidth();
        int height = getHeight();

        for (int i = 1; i < numColumns; i++) {
            canvas.drawLine(i * cellWidth, 0, i * cellWidth, height, redPaint);
        }

        for (int i = 1; i < numRows; i++) {
            canvas.drawLine(0, i * cellHeight, width, i * cellHeight, redPaint);
        }
        //canvas.restore();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        boolean dragged = true;
        switch (event.getAction() & MotionEvent.ACTION_MASK) {

            case MotionEvent.ACTION_DOWN:
                mode = DRAG;

                //We assign the current X and Y coordinate of the finger to startX and startY minus the previously translated
                //amount for each coordinates This works even when we are translating the first time because the initial
                //values for these two variables is zero.
                startX = event.getX() - previousTranslateX;
                startY = event.getY() - previousTranslateY;
                break;

            case MotionEvent.ACTION_MOVE:
                translateX = event.getX() - startX;
                translateY = event.getY() - startY;

                //We cannot use startX and startY directly because we have adjusted their values using the previous translation values.
                //This is why we need to add those values to startX and startY so that we can get the actual coordinates of the finger.
                double distance = Math.sqrt(Math.pow(event.getX() - (startX + previousTranslateX), 2) +
                                Math.pow(event.getY() - (startY + previousTranslateY), 2)
                );

                if(distance > 0) {
                    dragged = true;
                }

                break;

            case MotionEvent.ACTION_POINTER_DOWN:
                mode = ZOOM;
                break;

            case MotionEvent.ACTION_UP:
                mode = NONE;
                dragged = false;

                //All fingers went up, so let's save the value of translateX and translateY into previousTranslateX and
                //previousTranslate
                previousTranslateX = translateX;
                previousTranslateY = translateY;
                break;

            case MotionEvent.ACTION_POINTER_UP:
                mode = DRAG;

                //This is not strictly necessary; we save the value of translateX and translateY into previousTranslateX
                //and previousTranslateY when the second finger goes up
                previousTranslateX = translateX;
                previousTranslateY = translateY;
                break;
        }

        mScaleDetector.onTouchEvent(event);

        //We redraw the canvas only in the following cases:
        //
        // o The mode is ZOOM
        //        OR
        // o The mode is DRAG and the scale factor is not equal to 1 (meaning we have zoomed) and dragged is
        //   set to true (meaning the finger has actually moved)
        if ((mode == DRAG && mScaleFactor != 1f && dragged) || mode == ZOOM) {
            invalidate();
        }

        return true;
    }

    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            mScaleFactor *= detector.getScaleFactor();

            // Don't let the object get too small or too large.
            mScaleFactor = Math.max(1f, Math.min(mScaleFactor, 5.0f));

            invalidate();
            return true;
        }
    }
}