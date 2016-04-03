package com.example.shashwatsrivastava.androidapp549;

import android.databinding.DataBindingUtil;
import android.support.v4.app.DialogFragment;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.example.shashwatsrivastava.androidapp549.databinding.TagLayoutBinding;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.zip.Inflater;

public class MainActivity extends AppCompatActivity implements AddTagDialogFragment.DialogListener{
    private ArrayList<Tag> tags = new ArrayList<>();
    private HashSet<String> tagIDsSeen = new HashSet<>();
    private static final String TAG = "Tag";

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
            // Add the new Tag
            Tag newTag = new Tag(tagId, tagName);
            tags.add(newTag);
            tagIDsSeen.add(tagId);

            // Add new Tag to the layout

            ViewGroup viewGroup = (ViewGroup) this.findViewById(android.R.id.content);
            LayoutInflater layoutInflater = LayoutInflater.from(this);
            TagLayoutBinding binding = DataBindingUtil.inflate(layoutInflater, R.layout.tag_layout,
                    viewGroup, true);
            binding.setTag(newTag);

        } catch(Exception e) {
            e.printStackTrace();
        }

    }
}
