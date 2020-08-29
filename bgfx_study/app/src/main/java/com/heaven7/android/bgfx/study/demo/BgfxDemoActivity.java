package com.heaven7.android.bgfx.study.demo;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class BgfxDemoActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        NativeApi.initAssets(getApplicationContext(), getAssets());
        setContentView(getLayoutId());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        NativeApi.destroyAll();
    }

    protected int getLayoutId(){
        return R.layout.ac_bgfx_demo;
    }
}
