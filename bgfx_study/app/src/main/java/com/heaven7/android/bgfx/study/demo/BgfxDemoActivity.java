package com.heaven7.android.bgfx.study.demo;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.heaven7.android.hbmdx.Heaven7;

public class BgfxDemoActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        setContentView(getLayoutId());
    }

    protected int getLayoutId(){
        return R.layout.ac_bgfx_demo;
    }
}
