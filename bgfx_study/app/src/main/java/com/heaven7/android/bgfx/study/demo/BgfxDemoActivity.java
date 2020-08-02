package com.heaven7.android.bgfx.study.demo;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class BgfxDemoActivity extends AppCompatActivity {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("bgfx_study");
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.ac_bgfx_demo);
    }

}
