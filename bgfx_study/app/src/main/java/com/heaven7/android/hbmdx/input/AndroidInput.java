package com.heaven7.android.hbmdx.input;

import android.content.Context;
import android.hardware.SensorManager;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
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
    public static void getRotationMatrixFromVector(float[] r, float[] rotationVectorValues){
        SensorManager.getRotationMatrixFromVector(r, rotationVectorValues);
    }
    public static void getRotationMatrix(float[] R, float[] I, float[] gravity, float[] geomagnetic){
        SensorManager.getRotationMatrix(R, I, gravity, geomagnetic);
    }

    public AndroidInput() {
        nativePtr = nAlloc();
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

    private static native long nAlloc();
    private static native void nDealloc(long ptr);
    private static native void nOnTouch(long inputPtr, long mePtr, MotionEventWrapper wrapper);
    private static native boolean nOnKey(long inputPtr, long kePtr, int keyCode, KeyEventWrapper wrapper);
}

