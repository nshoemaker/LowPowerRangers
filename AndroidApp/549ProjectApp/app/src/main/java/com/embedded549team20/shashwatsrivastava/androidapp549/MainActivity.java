package com.embedded549team20.shashwatsrivastava.androidapp549;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.databinding.DataBindingUtil;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
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
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

import com.embedded549team20.shashwatsrivastava.androidapp549.databinding.TagLayoutBinding;
import com.squareup.picasso.Picasso;
import com.squareup.picasso.Transformation;

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
    private static final int CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE = 100;
    public static final int MEDIA_TYPE_IMAGE = 1;
    public static final int MEDIA_TYPE_VIDEO = 2;
    private Uri fileUri;

    // Storage Permissions
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };


    /** Create a file Uri for saving an image or video */
    private Uri getOutputMediaFileUri(int type){
        return Uri.fromFile(getOutputMediaFile(type));
    }

    private File getOutputMediaFile(int type){
        // To be safe, you should check that the SDCard is mounted
        // using Environment.getExternalStorageState() before doing this.

        File mediaStorageDir = new File(Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_PICTURES), "MyCameraApp");
        // This location works best if you want the created images to be shared
        // between applications and persist after your app has been uninstalled.
        // Create the storage directory if it does not exist
        if (! mediaStorageDir.exists()){
            if (! mediaStorageDir.mkdirs()){
                Log.d("MyCameraApp", "failed to create directory");
                return null;
            }
        }

        // Create a media file name
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        File mediaFile;
        if (type == MEDIA_TYPE_IMAGE){
            mediaFile = new File(mediaStorageDir.getPath() + File.separator +
                    "IMG_"+ timeStamp + ".jpg");
        } else if(type == MEDIA_TYPE_VIDEO) {
            mediaFile = new File(mediaStorageDir.getPath() + File.separator +
                    "VID_"+ timeStamp + ".mp4");
        } else {
            return null;
        }

        return mediaFile;
    }

    /**
     * Checks if the app has permission to write to device storage
     *
     * If the app does not has permission then the user will be prompted to grant permissions
     *
     * @param activity
     */
    public static void verifyStoragePermissions(Activity activity) {
        // Check if we have write permission
        int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permission != PackageManager.PERMISSION_GRANTED) {
            // We don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    activity,
                    PERMISSIONS_STORAGE,
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }

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

    private void resetView(){
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

        Button cameraButton = (Button) findViewById(R.id.camera_button);
        cameraButton.setBackgroundResource(R.drawable.camera_icon);
        cameraButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startCameraIntent();
            }
        });

        for(Tag tag: tagsSeen){
            // android.R.id.content gives you the root view
            ViewGroup viewGroup = (ViewGroup) findViewById(android.R.id.content);
            LayoutInflater layoutInflater = LayoutInflater.from(MainActivity.this);
            TagLayoutBinding binding = DataBindingUtil.inflate(layoutInflater, R.layout.tag_layout,
                    viewGroup, true);
            binding.setTag(tag);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        detector = new ScaleGestureDetector(this, new ScaleListener());
        verifyStoragePermissions(this);
        resetView();
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

    private void startCameraIntent(){
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        fileUri = getOutputMediaFileUri(MEDIA_TYPE_IMAGE);
        intent.putExtra(MediaStore.EXTRA_OUTPUT, fileUri);

        startActivityForResult(intent, CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        setContentView(R.layout.image_view);
        ImageView imageView = (ImageView) findViewById(R.id.image_view);

        // fileUri
        Picasso.with(this).load(fileUri).transform(new ShowOnImageTransformation()).into(imageView);
        Log.d("FileUri", fileUri.toString());

        Button cancelButton = (Button) findViewById(R.id.cancel_button);
        cancelButton.setBackgroundResource(R.drawable.cancel_button);
        cancelButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resetView();
            }
        });
    }

    private void changeTextBoxMessage(){
        PixelGridView pixelGridView = (PixelGridView) findViewById(R.id.pixel_view);
        TextView dimensions = (TextView) findViewById(R.id.dimensions_position);
        float cellHeight = (baseRoomHeight / pixelGridView.getNumRows()) / scaleFactor;
        String dimensionsMessage = "Each square is " +
                String.format("%.1f",cellHeight) + "cm by " +
                String.format("%.1f",cellHeight) + "cm";
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

    private class ShowOnImageTransformation implements Transformation {
        @Override
        public Bitmap transform(Bitmap bitmap) {
            Bitmap resultBitmap = bitmap.copy(bitmap.getConfig(), true);
            Canvas canvas = new Canvas(resultBitmap);
            Paint paint = new Paint();
            paint.setColor(Color.BLUE);
            paint.setStrokeWidth(10);
            for(Tag tag : tagsSeen){
                Float R = tag.R.get();
                Float theta = tag.theta.get();
                if(theta > 0){
                    if(theta > -Math.PI / 4)
                    {
                        Float Y = (float) ((1 / Math.tan(Math.abs(theta))) * bitmap.getWidth() / 2);
                        canvas.drawLine(bitmap.getWidth()/2, bitmap.getHeight(), 0, Y, paint);
                    }else{
                        Float X = (float) (Math.tan(Math.abs(theta)) * bitmap.getHeight());
                        canvas.drawLine(bitmap.getWidth()/2, bitmap.getHeight(), bitmap.getWidth()/2 - X, 0, paint);
                    }

                } else{
                    if(theta < Math.PI / 4){
                        Float X = (float) (Math.tan(Math.abs(theta)) * bitmap.getHeight());
                        canvas.drawLine(bitmap.getWidth()/2, bitmap.getHeight(), bitmap.getWidth()/2 + X, 0, paint);
                    } else{
                        Float Y = (float) ((1 / Math.tan(Math.abs(theta))) * bitmap.getWidth() / 2);
                        canvas.drawLine(bitmap.getWidth()/2, bitmap.getHeight(), bitmap.getWidth(), Y, paint);
                    }

                }
            }
            //canvas.drawLine(0, resultBitmap.getHeight()/2, resultBitmap.getWidth(), resultBitmap.getHeight()/2, paint);
            bitmap.recycle();
            return resultBitmap;
        }

        @Override
        public String key() {
            return "ShowOnImage";
        }
    }

}
