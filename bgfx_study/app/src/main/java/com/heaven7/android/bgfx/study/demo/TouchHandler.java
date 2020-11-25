package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;

import com.heaven7.android.hbmdx.input.AndroidInput;

public class TouchHandler implements View.OnTouchListener,View.OnKeyListener{

    private final AndroidInput input = new AndroidInput();

    @Override
    public boolean onTouch(View v, MotionEvent event) {

        input.onTouch(event);
        //Gdx.app.getGraphics().requestRendering();
        return true;
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        //input.onKey();
        return false;
    }

    public boolean supportsMultitouch (Context activity) {
        return activity.getPackageManager().hasSystemFeature("android.hardware.touchscreen.multitouch");
    }
}
