package com.example.shashwatsrivastava.androidapp549;

import android.databinding.DataBindingUtil;
import android.support.v4.app.DialogFragment;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.example.shashwatsrivastava.androidapp549.databinding.TagLayoutBinding;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity implements AddTagDialogFragment.DialogListener{
    private HashMap<String, String> tagIDsSeen = new HashMap<String,String>();
    private static final String TAG = "InMain";
    private String url = "https://test-server-549.herokuapp.com/testServer/get/";
    private String tagId;
    private String tagName;
    private HashSet<Tag> tagsSeen = new HashSet<>();
    private float scaleFactor = 1.f;
    private ScaleGestureDetector detector;
    private final int baseRoomHeight = 800;

    private Callback customCallback = new Callback() {
        @Override
        public void onFailure(Call call, IOException e) {
            // Something went wrong
            Log.d(TAG, "The call went wrong");
        }

        @Override
        public void onResponse(Call call, Response response) throws IOException {
            String reponseString;
            if(response.isSuccessful()) {
                reponseString = response.body().string();

                try {
                    // Check if the tag is valid
                    JSONObject jsonObject = new JSONObject(reponseString);
                    jsonObject.getDouble("theta");

                    // Add new Tag to the layout
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Tag newTag = new Tag(tagId, tagName);
                            tagsSeen.add(newTag);
                            tagIDsSeen.put(tagId, tagName);
                            // android.R.id.content gives you the root view
                            ViewGroup viewGroup = (ViewGroup) findViewById(android.R.id.content);
                            LayoutInflater layoutInflater = LayoutInflater.from(MainActivity.this);
                            TagLayoutBinding binding = DataBindingUtil.inflate(layoutInflater, R.layout.tag_layout,
                                    viewGroup, false);
                            binding.setTag(newTag);
                            viewGroup.addView(binding.getRoot());
                            Log.d("ERROR", "Added the tag");
                        }
                    });

                } catch (JSONException e) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.this, R.string.tag_doesnt_exist, Toast.LENGTH_LONG).show();
                        }
                    });
                    e.printStackTrace();
                }
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        detector = new ScaleGestureDetector(this, new ScaleListener());
        setContentView(R.layout.activity_main);

        changeTextBoxMessage();

        // Setup FAB
        FloatingActionButton addItemFab = (FloatingActionButton) findViewById(R.id.add_item_fab);
        addItemFab.setImageResource(R.drawable.ic_add_white_24dp);
        addItemFab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DialogFragment dialogFragment = new AddTagDialogFragment();
                dialogFragment.show(getSupportFragmentManager(), "AddTag");
            }
        });

        if(savedInstanceState != null){
            this.tagIDsSeen = (HashMap<String, String>)savedInstanceState.getSerializable("Tags");
            for(String tagId: tagIDsSeen.keySet()){
                Tag newTag = new Tag(tagId, tagIDsSeen.get(tagId));
                tagsSeen.add(newTag);
                // android.R.id.content gives you the root view
                ViewGroup viewGroup = (ViewGroup) findViewById(android.R.id.content);
                LayoutInflater layoutInflater = LayoutInflater.from(MainActivity.this);
                TagLayoutBinding binding = DataBindingUtil.inflate(layoutInflater, R.layout.tag_layout,
                        viewGroup, true);
                binding.setTag(newTag);
            }
        }
    }

    private void changeTextBoxMessage(){
        PixelGridView pixelGridView = (PixelGridView) findViewById(R.id.pixel_view);
        TextView dimensions = (TextView) findViewById(R.id.dimensions_position);
        float cellHeight = (baseRoomHeight / pixelGridView.getNumRows()) / scaleFactor;
        String dimensionsMessage = "Each square is " + cellHeight + "cm by " + cellHeight + "cm";
        dimensions.setText(dimensionsMessage);
        Log.d(TAG, dimensionsMessage);
    }

    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState){
        super.onSaveInstanceState(savedInstanceState);
        savedInstanceState.putSerializable("Tags", tagIDsSeen);
    }

    @Override
    public void onPositiveClick(String tagId, String tagName) {
        if(tagIDsSeen.containsKey(tagId)){
            Toast.makeText(this, R.string.tagID_already_seen, Toast.LENGTH_LONG).show();
            return;
        }
        try{

            this.tagId = tagId;
            this.tagName = tagName;
            makeGetRequest(url + tagId, customCallback);


        } catch(Exception e) {
            e.printStackTrace();
        }

    }

    private Call makeGetRequest(String url, Callback callback) throws IOException {
        Log.d(TAG, "Making get request for url " + url);
        OkHttpClient client =  new OkHttpClient();
        Request request = new Request.Builder()
                .url(url)
                .build();

        Call call = client.newCall(request);
        call.enqueue(callback);
        return call;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        detector.onTouchEvent(event);
        return true;
    }

    private void changeTagRoomSizes(float factor){
        for(Tag tag : tagsSeen){
            tag.setRoomHeight(factor);
        }
    }

    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            scaleFactor *= detector.getScaleFactor();
            scaleFactor = Math.max(0.3f, Math.min(5.0f, scaleFactor));
            changeTagRoomSizes(scaleFactor);
            changeTextBoxMessage();
            return true;
        }
    }

}
