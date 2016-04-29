package com.embedded549team20.shashwatsrivastava.androidapp549;

import android.animation.ObjectAnimator;
import android.content.Context;
import android.databinding.BaseObservable;
import android.databinding.Bindable;
import android.databinding.BindingAdapter;
import android.databinding.ObservableFloat;
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

/**
 * Created by shashwatsrivastava on 4/1/16.
 *
 * Tag class which controls all the Tag functions
 */
public class Tag extends BaseObservable {
    private int tagID;
    private static final String TAG = "TagInTag";
    private static final int UPDATE_MILLIS = 300;
    private String tagName;
    public ObservableFloat translationX;
    public ObservableFloat translationY;
    private float dpToPx;
    private float dpHeight;
    private float dpWidth;
    // Last updated coordinates
    private float targetX;
    private float targetY;
    // Need size of tag view to get correct position of tag cause otherwise it shows tag left rather
    // than tag center
    private int tagViewHeight = 45;
    private int tagViewWidth = 100;
    private OkHttpClient client = new OkHttpClient();
    private String url = "https://test-server-549.herokuapp.com/testServer/get/";
    private String reponseString;
    public ObservableFloat theta;
    public ObservableFloat R;
    // Assuming size of room is 8m
    private float baseRoomHeight = 800;
    private float roomHeight = 800;
    private float roomWidth;
    private Context context;
    private ArrayList<Float> arrayX = new ArrayList<>();
    private ArrayList<Float> arrayY = new ArrayList<>();

    private Callback customCallback = new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                // Something went wrong
            }

            @Override
            public void onResponse(Call call, Response response)  {
                if(response.isSuccessful()){
                    try{
                        reponseString = response.body().string();
                        JSONObject jsonObject = new JSONObject(reponseString);
                        theta.set((float) jsonObject.getDouble("theta"));
                        R.set((float) jsonObject.getDouble("R"));
                        setNewTagPosition();
                    } catch(JSONException e){
                        Log.d(TAG, "Error in JSON");
                        e.printStackTrace();
                    } catch(Exception e){
                        e.printStackTrace();
                    }
                }
            }
        };

    @BindingAdapter({"app:targetX", "app:targetY"})
    public static void animateTranslation(LinearLayout layout, float x, float y) {
        ObjectAnimator animX = ObjectAnimator.ofFloat(layout, "x", layout.getX(), x);
        ObjectAnimator animY = ObjectAnimator.ofFloat(layout, "y", layout.getY(), y);
        animX.setDuration(UPDATE_MILLIS * 3);
        animY.setDuration(UPDATE_MILLIS * 3);
        animX.start();
        animY.start();
    }

    private Float getAverageOfArray(ArrayList<Float> arr){
        Float total = 0.0f;
        for(Float num : arr){
            total += num;
        }
        return total / arr.size();
    }

    /**
     * This method uses the values of R and theta to
     * calculate the new position for the tag dot
     */
    private void setNewTagPosition() {
        targetX = (float) ((this.dpWidth/2 - tagViewWidth/2 + Math.sin(theta.get()) * R.get() * (this.dpWidth / this.roomWidth)) * this.dpToPx);
        targetY = (float) (((Math.cos(theta.get()) * R.get() * (this.dpHeight / this.roomHeight)) - 0.75 * tagViewHeight) * this.dpToPx);
//        Log.d(TAG, "Theta is " + theta);
//        Log.d(TAG, "R is " + R);
//        Log.d(TAG, "deltaX is " + deltaX);
//        Log.d(TAG, "deltaY is " + deltaY);
        if(arrayX.size() >= 5){
            arrayX.remove(0);
        }
        if(arrayY.size() >= 5){
            arrayY.remove(0);
        }
        arrayX.add(targetX);
        arrayY.add(targetY);
        Log.d(TAG, Double.toString(Math.cos(theta.get()) * R.get()));
        Log.d(TAG, Double.toString((this.dpHeight / this.roomHeight)));
        Log.d(TAG, Double.toString(0.75 * tagViewHeight));
        if(arrayX.size() >= 5){
            translationX.set(getAverageOfArray(arrayX));
        } else {
            translationX.set(targetX);
        }
        if(arrayY.size() >= 5){
            translationY.set(getAverageOfArray(arrayY));
        } else{
            translationY.set(targetY);
        }
    }

    public Tag(String tagID, String tagName) {
        this.tagID = Integer.parseInt(tagID);
        this.tagName = tagName;
        this.dpToPx = DisplayUtilities.dpToPx(1);
        this.dpHeight = DisplayUtilities.getDpHeight();
        this.dpWidth = DisplayUtilities.getDpWidth();
        this.targetX = dpWidth/2 * this.dpToPx - tagViewWidth;
        this.targetY = dpHeight/2 * this.dpToPx - tagViewHeight;
        this.translationX = new ObservableFloat(targetX);
        this.translationY = new ObservableFloat(targetY);
        this.theta = new ObservableFloat(0);
        this.R = new ObservableFloat(0);
        this.url = this.url + tagID;
        this.roomWidth = this.roomHeight * (dpWidth/ dpHeight);
        makeGetRequest(customCallback);
//        this.context = context;

        Log.d(TAG, "Height in dp is " + this.dpHeight);
        Log.d(TAG, "Width in dp is " + this.dpWidth);
        Log.d(TAG, "dpToPX is " + this.dpToPx);
        Timer timer = new Timer();
        timer.schedule(new UpdateTagValues(this), 0, UPDATE_MILLIS);
    }

    @Bindable
    public String getTagName(){
        return this.tagName;
    }

    private Call makeGetRequest(Callback callback) {
        Request request = new Request.Builder()
                .url(this.url)
                .build();

        Call call = client.newCall(request);
        call.enqueue(callback);
        return call;
    }

    public View.OnClickListener clickListener = new View.OnClickListener() {
        /**
         * This functions open a Toast which displays the info
         * for the given tag
         */
        @Override
        public void onClick(View view) {
            TextView r = (TextView) view.findViewById(com.embedded549team20.shashwatsrivastava.androidapp549.R.id.tag_info_r);
            r.setText("The distance from the center is " + Float.toString(Tag.this.R.get()));
            TextView theta = (TextView) view.findViewById(com.embedded549team20.shashwatsrivastava.androidapp549.R.id.tag_info_theta);
            theta.setText("Theta is " + Float.toString(Tag.this.theta.get()));
            final LinearLayout linearLayout = (LinearLayout) view.findViewById(com.embedded549team20.shashwatsrivastava.androidapp549.R.id.layout_tag_info);
            if(linearLayout.getVisibility() == View.VISIBLE){
                linearLayout.setVisibility(View.INVISIBLE);
            }else{
                linearLayout.setVisibility(View.VISIBLE);
            }
        }
    };

    public void setRoomHeight(float factor){
        roomHeight = baseRoomHeight / factor;
        Log.d(TAG, "The room height is ");
        Log.d(TAG, Float.toString(roomHeight));
        roomWidth = this.roomHeight * (dpWidth/ dpHeight);
        setNewTagPosition();
    }

    private class UpdateTagValues extends TimerTask {
        private Tag tag;
        private static final String TAG = "UpdateValueTag";

        public UpdateTagValues(Tag tag){
            this.tag = tag;
        }

        @Override
        public void run() {
            try{
                makeGetRequest(customCallback);
//                tag.translationX.set(tag.translationX.get() + 10);
//                tag.translationY.set(tag.translationY.get() + 5);
//                Log.d(TAG, Float.toString(tag.translationX.get()));
//                Log.d(TAG, Float.toString(tag.translationY.get()));
            } catch(Exception e){
                e.printStackTrace();
            }

        }
    }
}
