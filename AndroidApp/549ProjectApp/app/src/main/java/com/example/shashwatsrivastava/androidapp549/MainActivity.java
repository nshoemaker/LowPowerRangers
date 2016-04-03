package com.example.shashwatsrivastava.androidapp549;

import android.support.v4.app.DialogFragment;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;

public class MainActivity extends AppCompatActivity implements AddTagDialogFragment.DialogListener{
    private ArrayList<Tag> tags = new ArrayList<>();
    private HashSet<String> tagIDsSeen = new HashSet<>();

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
            Tag newTag = new Tag(tagId, tagName);
            tags.add(newTag);
            tagIDsSeen.add(tagId);
        } catch(Exception e) {
            e.printStackTrace();
        }

    }
}
