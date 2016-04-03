package com.example.shashwatsrivastava.androidapp549;

import android.databinding.BaseObservable;
import android.databinding.Bindable;
import android.databinding.ObservableFloat;
import android.databinding.ObservableInt;
import android.util.Log;

import com.example.shashwatsrivastava.androidapp549.BR;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
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
    private String tagName;
    public ObservableFloat translationX;
    public ObservableFloat translationY;
    private float dpToPx;
    private float dpHeight;
    private float dpWidth;
    // Need size of tag view to get correct position of tag cause otherwise it shows tag left rather
    // than tag center
    private int tagViewHeight = 50;
    private int tagViewWidth = 100;
    private OkHttpClient client = new OkHttpClient();
    private String url = "https://test-server-549.herokuapp.com/testServer/get/";
    private String reponseString;
    private double theta;
    private double R;
    // TODO: Add some kind of error checking if Tag doesnt exist
    private boolean errorSeen = false;
    // Assuming size of room is 8m
    private final int roomWidth = 8;
    private final int roomHeight = 8;

    private Callback customCallback = new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                // Something went wrong
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if(response.isSuccessful()){
                    reponseString = response.body().string();
                    try{
                        JSONObject jsonObject = new JSONObject(reponseString);
                        theta = jsonObject.getDouble("theta");
                        R = jsonObject.getDouble("R");
                        setNewTagPosition();
                    } catch(JSONException e){
                        Log.d(TAG, "Error in JSON");
                        e.printStackTrace();
                    }
                }
            }
        };

    /**
     * This method uses the values of R and theta to
     * calculate the new position for the tag dot
     */
    private void setNewTagPosition() {
        float deltaX = (float) ((this.dpWidth/2 + Math.sin(theta) * R * (this.dpWidth / this.roomWidth)) * this.dpToPx) - tagViewWidth;
        float deltaY = Math.abs((float) ((Math.cos(theta) * R * (this.dpHeight / this.roomHeight)) * this.dpToPx) - tagViewHeight);
        translationX.set(deltaX);
        translationY.set(deltaY);
    }

    public Tag(String tagID, String tagName, float px, float dpHeight, float dpWidth) throws IOException, TagErrorException {
        this.tagID = Integer.parseInt(tagID);
        this.tagName = tagName;
        this.dpToPx = px;
        this.dpHeight = dpHeight;
        this.dpWidth = dpWidth;
        this.translationX = new ObservableFloat(dpWidth/2 * px - tagViewWidth);
        this.translationY = new ObservableFloat(dpHeight/2 * px - tagViewHeight);
        this.url = this.url + tagID;
        makeGetRequest(customCallback);

        Timer timer = new Timer();
        timer.schedule(new UpdateTagValues(this), 0, 1000);
    }

    @Bindable
    public String getTagName(){
        return this.tagName;
    }

    private Call makeGetRequest(Callback callback) throws IOException {
        Request request = new Request.Builder()
                .url(this.url)
                .build();

        Call call = client.newCall(request);
        call.enqueue(callback);
        return call;
    }

    // TODO: Use this to signal errors when tag isnt found
    public class TagErrorException extends Exception {
        public TagErrorException(){
            super();
        }

        public TagErrorException(String message){
            super(message);
        }
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
                Log.d(TAG, Float.toString(tag.translationX.get()));
                Log.d(TAG, Float.toString(tag.translationY.get()));
            } catch(Exception e){
                e.printStackTrace();
            }

        }
    }
}
