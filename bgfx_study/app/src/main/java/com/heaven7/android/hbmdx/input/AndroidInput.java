package com.heaven7.android.hbmdx.input;

import android.app.Activity;
import android.content.Context;
import android.hardware.SensorManager;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import androidx.annotation.Keep;

import com.heaven7.android.bgfx.study.demo.BgfxLuaView;
import com.heaven7.core.util.MainWorker;
import com.heaven7.java.base.anno.CalledInternal;

import java.lang.ref.WeakReference;

public final class AndroidInput {

    private long nativePtr;
    private WeakReference<BgfxLuaView> mWeakView;
    private Context mAppContext;

    public void onTouch(MotionEvent event) {
        MotionEventWrapper wrapper = MotionEventWrapper.obtain();
        wrapper.setEvent(MotionEvent.obtain(event));
        nOnTouch(getNativePtr(), wrapper.getNativePtr(), wrapper);
    }
    public boolean onKey(int keyCode, KeyEvent event) {
        KeyEventWrapper wrapper = KeyEventWrapper.obtain();
        wrapper.setEvent(new KeyEvent(event));
        return nOnKey(getNativePtr(), wrapper.getNativePtr(), keyCode, wrapper);
    }

    public void setView(BgfxLuaView view){
        mWeakView = new WeakReference<>(view);
        mAppContext = view.getContext().getApplicationContext();
    }

    @CalledInternal
    @Keep //native
    public static void setOnscreenKeyboardVisible(final AndroidInput input, final boolean visible, final int type) {
        MainWorker.post(new Runnable() {
            public void run () {
                BgfxLuaView luaView = input.mWeakView.get();
                if(luaView == null){
                    return;
                }
                InputMethodManager manager = (InputMethodManager)input.mAppContext.getSystemService(Context.INPUT_METHOD_SERVICE);
                if (visible) {
                    if(luaView.getOnscreenKeyboardType() != type) {
                        luaView.setOnscreenKeyboardType(type);
                        manager.restartInput(luaView);
                    }
                    luaView.setFocusable(true);
                    luaView.setFocusableInTouchMode(true);
                    manager.showSoftInput(luaView, 0);
                } else {
                    manager.hideSoftInputFromWindow(luaView.getWindowToken(), 0);
                }
            }
        });
    }
    @Keep
    public static void getRotationMatrixFromVector(float[] r, float[] rotationVectorValues){
        SensorManager.getRotationMatrixFromVector(r, rotationVectorValues);
    }
    @Keep
    public static boolean getRotationMatrix(float[] R, float[] I, float[] gravity, float[] geomagnetic){
        return SensorManager.getRotationMatrix(R, I, gravity, geomagnetic);
    }
    @Keep
    public static void getOrientation(float[] R, float[] orientation){
        SensorManager.getOrientation(R, orientation);
    }
    @Keep
    public static int getRotation(AndroidInput input) {
        BgfxLuaView luaView = input.mWeakView.get();
        if(luaView == null){
            return 0;
        }
        Context context = luaView.getContext();
        int orientation = 0;
        if (context instanceof Activity) {
            orientation = ((Activity)context).getWindowManager().getDefaultDisplay().getRotation();
        } else {
            orientation = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
        }
        switch (orientation) {
            case Surface.ROTATION_0:
                return 0;
            case Surface.ROTATION_90:
                return 90;
            case Surface.ROTATION_180:
                return 180;
            case Surface.ROTATION_270:
                return 270;
            default:
                return 0;
        }
    }

    public AndroidInput() {
        nativePtr = nAlloc(this);
    }

    public long getNativePtr() {
        return nativePtr;
    }
    @Override
    protected void finalize() throws Throwable {
        if (nativePtr != 0) {
            nDealloc(nativePtr);
            nativePtr = 0;
        }
        super.finalize();
    }

    private static native long nAlloc(AndroidInput input);
    private static native void nDealloc(long ptr);
    private static native void nOnTouch(long inputPtr, long mePtr, MotionEventWrapper wrapper);
    private static native boolean nOnKey(long inputPtr, long kePtr, int keyCode, KeyEventWrapper wrapper);
}

