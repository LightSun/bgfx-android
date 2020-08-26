package com.heaven7.android.bgfx.study.demo;

import android.Manifest;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class MainActivity extends AppCompatActivity {

   /* static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("bgfx_core");
    }*/

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        }else {
            toBgfxDemo();
        }
    }

    private void toBgfxDemo() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //startActivity(new Intent(MainActivity.this, BgfxDemoActivity.class));
                startActivity(new Intent(MainActivity.this, BgfxLuaActivity.class));
            }
        });
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        switch (requestCode) {
            case 1: {
                if (grantResults.length > 0) {
                    if(grantResults[0] != PackageManager.PERMISSION_GRANTED){
                        // Permission refused, show a warning!
                        AlertDialog alertDialog = new AlertDialog.Builder(this).create();
                        alertDialog.setTitle("Warning");
                        alertDialog.setMessage("Some examples needs to load data from the sdcard, those will crash if data can't be loaded!");
                        alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        });
                        alertDialog.show();
                    }else {
                        toBgfxDemo();
                    }
                }
            }
        }
    }
}