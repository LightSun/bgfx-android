package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class BgfxSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "BgfxSurfaceView";
    private Thread mThread;

    public BgfxSurfaceView(Context context) {
        this(context, null);
    }

    public BgfxSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        getHolder().addCallback(this);
    }
    private void start(){
        if(mThread == null){
            mThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    long start = System.currentTimeMillis();
                    long end;
                    //keep 60-fps
                    Log.d(TAG, "loop draw >>> start");
                    while (NativeApi.update()){ //TODO native crashed.
                        end = System.currentTimeMillis();
                        if(end - start < 16){
                            SystemClock.sleep(end - start);
                            start = end;
                        }
                    }
                    Log.d(TAG, "loop draw >>>  end");
                }
            });
        }
        mThread.start();
    }
    private void destroyThread(){
        if(mThread != null){
            mThread.interrupt();
            mThread = null;
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        NativeApi.initializeSurface(this, holder.getSurface());
       // start(); //TODO bgfx can only other thread
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
       //TODO later use
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        destroyThread();
        NativeApi.destroySurface(this, holder.getSurface());
    }
}
