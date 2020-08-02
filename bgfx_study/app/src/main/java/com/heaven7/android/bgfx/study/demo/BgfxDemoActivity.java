package com.heaven7.android.bgfx.study.demo;

import android.app.NativeActivity;
import android.os.Bundle;

import androidx.annotation.Nullable;

public class BgfxDemoActivity extends NativeActivity{

    static {
        System.loadLibrary("bgfx_study");
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

}
