package com.example.shashwatsrivastava.androidapp549;

import android.databinding.BaseObservable;
import android.databinding.Bindable;
import android.util.Log;

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
 */
public class Tag extends BaseObservable {
    private int tagID;
    private static final String TAG = "Tag";
    private String tagName;
    private String paddingLeft;
    private OkHttpClient client = new OkHttpClient();
    private String url = "https://test-server-549.herokuapp.com/testServer/get/";
    private String reponseString;
    private double theta;
    private double R;
    private boolean errorSeen = false;

    private Callback customCallback = new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                // Something went wrong
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if(response.isSuccessful()){
                    reponseString = response.body().string();
                    Log.d(TAG, reponseString);
                    try{
                        JSONObject jsonObject = new JSONObject(reponseString);
                        theta = jsonObject.getDouble("theta");
                        R = jsonObject.getDouble("R");
                    } catch(JSONException e){
                        Log.d(TAG, "Error in JSON");
                        e.printStackTrace();
                    }
                }
            }
        };

    public Tag(String tagID, String tagName) throws IOException, TagErrorException {
        this.tagID = Integer.parseInt(tagID);
        this.tagName = tagName;
        this.paddingLeft = "30dp";
        this.url = this.url + tagID;
        Call reply = makeGetRequest(customCallback);

        Timer timer = new Timer();
        timer.schedule(new UpdateTagValues(), 0, 1000);
    }

    @Bindable
    public String getTagName(){
        return this.tagName;
    }

    @Bindable
    public String getPaddingLeft(){
        return this.paddingLeft;
    }

    private Call makeGetRequest(Callback callback) throws IOException {
        Request request = new Request.Builder()
                .url(this.url)
                .build();

        Call call = client.newCall(request);
        call.enqueue(callback);
        return call;
    }

    public class TagErrorException extends Exception {
        public TagErrorException(){
            super();
        }

        public TagErrorException(String message){
            super(message);
        }
    }

    private class UpdateTagValues extends TimerTask {

        @Override
        public void run() {
            try{
                makeGetRequest(customCallback);
            } catch(Exception e){
                e.printStackTrace();
            }

        }
    }
}
