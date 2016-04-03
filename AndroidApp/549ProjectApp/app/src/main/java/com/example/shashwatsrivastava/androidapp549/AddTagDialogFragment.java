package com.example.shashwatsrivastava.androidapp549;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.support.v4.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

/**
 * Created by shashwatsrivastava on 4/1/16.
 */
public class AddTagDialogFragment extends DialogFragment{
    private DialogListener listener;

    @Override
    public void onAttach(Activity activity){
        super.onAttach(activity);

        listener = (DialogListener) activity;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstance){
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        LayoutInflater layoutInflater = getActivity().getLayoutInflater();

        View view = layoutInflater.inflate(R.layout.dialog_add_tag, null);
        final EditText tagId = (EditText) view.findViewById(R.id.task_EnterTagID);
        final EditText tagName = (EditText) view.findViewById(R.id.task_EnterName);

        builder.setView(view)
                .setMessage(R.string.dialog_message)
                .setPositiveButton(R.string.dialog_save, new DialogInterface.OnClickListener(){

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        listener.onPositiveClick(tagId.getText().toString(), tagName.getText().toString());
                    }
                });

        return builder.create();
    }

    /**
     * Interface used to let a listener know what task the user wants to add.
     */
    public interface DialogListener {
        void onPositiveClick(String tagId, String tagName);
    }
}
