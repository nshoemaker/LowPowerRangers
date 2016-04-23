package com.example.shashwatsrivastava.androidapp549;

import android.content.res.Resources;
import android.databinding.DataBindingUtil;
import android.support.v4.app.DialogFragment;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.example.shashwatsrivastava.androidapp549.databinding.TagLayoutBinding;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.HashSet;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity implements AddTagDialogFragment.DialogListener{
    private HashSet<String> tagIDsSeen = new HashSet<>();
    private static final String TAG = "Tag";
    private String url = "https://test-server-549.herokuapp.com/testServer/get/";
    private String tagId;
    private String tagName;

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
                            Tag newTag = new Tag(tagId, tagName, getApplicationContext());
                            tagIDsSeen.add(tagId);
                            // android.R.id.content gives you the root view
                            ViewGroup viewGroup = (ViewGroup) findViewById(android.R.id.content);
                            LayoutInflater layoutInflater = LayoutInflater.from(MainActivity.this);
                            TagLayoutBinding binding = DataBindingUtil.inflate(layoutInflater, R.layout.tag_layout,
                                    viewGroup, true);
                            binding.setTag(newTag);
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
        setContentView(R.layout.activity_main);

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


    }

    @Override
    public void onPositiveClick(String tagId, String tagName) {
        if(tagIDsSeen.contains(tagId)){
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

}
