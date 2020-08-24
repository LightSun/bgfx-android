package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.heaven7.android.bgfx.study.demo.lua.Luaer;

public class BgfxLuaView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "BgfxLuaView";
    private boolean mInited;
    private Runnable mPendingTask;

    public BgfxLuaView(Context context) {
        this(context, null);
    }

    public BgfxLuaView(Context context, AttributeSet attrs) {
        super(context, attrs);

        getHolder().addCallback(this);
    }

    public void setScriptFile(final Luaer mLuaer, final String assetPath) {
        Log.d(TAG, "setScriptFile: path = " + assetPath);
        NativeApi.destroySurface(this);
        if(mInited){
            post(new Runnable() {
                @Override
                public void run() {
                    mLuaer.loadLuaAssets(assetPath);
                }
            });
        }else {
            mPendingTask = new Runnable() {
                @Override
                public void run() {
                    mLuaer.loadLuaAssets(assetPath);
                }
            };
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
        mInited = true;
        if(mPendingTask != null){
            Runnable task = mPendingTask;
            mPendingTask = null;
            post(task);
        }
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        //TODO later use
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        NativeApi.destroySurface(this);
    }
}
