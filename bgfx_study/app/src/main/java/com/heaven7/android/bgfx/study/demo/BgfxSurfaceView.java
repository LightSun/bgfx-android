package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class BgfxSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private Thread mThread;

    public BgfxSurfaceView(Context context) {
        this(context, null);
    }

    public BgfxSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        getHolder().addCallback(this);
    }

    void startThread(){
        if(mThread == null){
            mThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    onDrawFrame();
                }
            });
            mThread.start();
        }
    }
    void destroyThread(){
        if(mThread != null){
            mThread.interrupt();
            mThread = null;
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        destroyThread();
        super.onDetachedFromWindow();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        initializeSurface(holder.getSurface());
        startThread();
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
       //TODO later use
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        destroyThread();
        destroySurface(holder.getSurface());
    }

    private native void initializeSurface(Surface surface);
    private native void destroySurface(Surface surface);
    private native void onDrawFrame();
}
