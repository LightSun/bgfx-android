package com.heaven7.android.bgfx.study.demo;

import android.Manifest;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.heaven7.core.util.PermissionHelper;

import java.util.List;

public class MainActivity extends AbsMainActivity {

    private PermissionHelper mHelper = new PermissionHelper(this);
    private boolean permit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestPermission();
    }
    private void requestPermission(){
        mHelper.startRequestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE,
                1, new PermissionHelper.ICallback() {
                    @Override
                    public void onRequestPermissionResult(String s, int i, boolean b) {
                        permit = b;
                    }
                    @Override
                    public boolean handlePermissionHadRefused(String s, int i, Runnable runnable) {
                        return false;
                    }
                });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        mHelper.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    protected void onClickItem(ActivityInfo info) {
        if(permit){
            super.onClickItem(info);
        }else {
            // Permission refused, show a warning!
            AlertDialog alertDialog = new AlertDialog.Builder(this).create();
            alertDialog.setTitle("Warning");
            alertDialog.setMessage("Some examples needs to load data from the sdcard, those will crash if data can't be loaded!");
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, "cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, "req permission", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    requestPermission();
                }
            });
            alertDialog.show();
        }
    }

    @Override
    protected void addDemos(List<ActivityInfo> list) {
        list.add(new ActivityInfo(BgfxLuaActivity.class, "bgfx lua demo"));
        list.add(new ActivityInfo(BgfxDemoActivity.class, "bgfx c++ demo"));
    }
}
