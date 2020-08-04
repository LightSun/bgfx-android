package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class BgfxSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "BgfxSurfaceView";

    public BgfxSurfaceView(Context context) {
        this(context, null);
    }

    public BgfxSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        getHolder().addCallback(this);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        NativeApi.initializeSurface(this, holder.getSurface());
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
       //TODO later use
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        NativeApi.destroySurface(this, holder.getSurface());
    }
}
