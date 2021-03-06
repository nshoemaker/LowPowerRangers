package com.embedded549team20.shashwatsrivastava.androidapp549;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.widget.RelativeLayout;

/**
 * Created by shashwatsrivastava on 4/21/16.
 */
public class PixelGridView extends RelativeLayout {
    private int numRows = 30;
    private int cellHeight = (int)DisplayUtilities.getDpHeight() / numRows;
    private int cellWidth = cellHeight;
    private int numColumns = (int) DisplayUtilities.getDpWidth() / cellWidth;
    private Paint redPaint = new Paint();

    public PixelGridView(Context context) {
        this(context, null);
        redPaint.setColor(Color.RED);
        setWillNotDraw(false);
    }

    public PixelGridView(Context context, AttributeSet attrs) {
        super(context, attrs);
        redPaint.setStyle(Paint.Style.FILL_AND_STROKE);
        redPaint.setColor(Color.RED);
        setWillNotDraw(false);
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
        super.onDraw(canvas);

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
    }



}