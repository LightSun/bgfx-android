package com.heaven7.android.bgfx.study.demo;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.SystemClock;
import android.text.InputType;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;

import com.heaven7.android.bgfx.study.demo.lua.Luaer;
import com.heaven7.android.hbmdx.input.InputCons;

public class BgfxLuaView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "BgfxLuaView";
    private boolean mInited;
    private Runnable mPendingTask;

    private int onscreenKeyboardType = InputCons.Default;

    public BgfxLuaView(Context context) {
        this(context, null);
    }

    public BgfxLuaView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public int getOnscreenKeyboardType() {
        return onscreenKeyboardType;
    }
    public void setOnscreenKeyboardType(int onscreenKeyboardType){
        this.onscreenKeyboardType = onscreenKeyboardType;
    }

    public void addPendingTask(Runnable task){
        if(mInited){
            post(task);
        }else {
            if(mPendingTask != null){
                Log.d(TAG, "mPendingTask not null");
                return;
            }
            mPendingTask = task;
        }
    }

    public void setScriptFile(final Luaer mLuaer, final String assetPath) {
        Log.d(TAG, "setScriptFile: path = " + assetPath);
        //NativeApi.destroySurface(this);
        if(mInited){
            post(new Runnable() {
                @Override
                public void run() {
                    //Log.d(TAG, "loadLuaAssets >>> threadId = " + Thread.currentThread().getId());
                    mLuaer.loadLuaAssets(assetPath);
                }
            });
        }else {
            mPendingTask = new Runnable() {
                @Override
                public void run() {
                   // Log.d(TAG, "loadLuaAssets >>> threadId = " + Thread.currentThread().getId());
                    mLuaer.loadLuaAssets(assetPath);
                }
            };
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        getHolder().addCallback(this);
    }

    @Override
    protected void onDetachedFromWindow() {
        getHolder().removeCallback(this);
        super.onDetachedFromWindow();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        //Log.d(TAG, "surfaceCreated >>> threadId = " + Thread.currentThread().getId());
        NativeApi.initializeSurface(this, holder.getSurface(), Luaer.get().getLuaPtr());
        mInited = true;
        if(mPendingTask != null){
            mPendingTask.run();
            mPendingTask = null;
        }
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        //TODO later use
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        mInited = false;
        NativeApi.destroySurface(this, Luaer.get().getLuaPtr());
    }

    @Override
    public InputConnection onCreateInputConnection (EditorInfo outAttrs) {

        // add this line, the IME can show the selectable words when use chinese input method editor.
        if (outAttrs != null) {
            outAttrs.imeOptions = outAttrs.imeOptions | EditorInfo.IME_FLAG_NO_EXTRACT_UI;
            outAttrs.inputType = getAndroidInputType(onscreenKeyboardType);
        }

        BaseInputConnection connection = new BaseInputConnection(this, false) {
            @Override
            public boolean deleteSurroundingText (int beforeLength, int afterLength) {
                int sdkVersion = android.os.Build.VERSION.SDK_INT;
                if (sdkVersion >= 16) {
                    /*
                     * In Jelly Bean, they don't send key events for delete. Instead, they send beforeLength = 1, afterLength = 0. So,
                     * we'll just simulate what it used to do.
                     */
                    if (beforeLength == 1 && afterLength == 0) {
                        sendDownUpKeyEventForBackwardCompatibility(KeyEvent.KEYCODE_DEL);
                        return true;
                    }
                }
                return super.deleteSurroundingText(beforeLength, afterLength);
            }

            @TargetApi(16)
            private void sendDownUpKeyEventForBackwardCompatibility (final int code) {
                final long eventTime = SystemClock.uptimeMillis();
                super.sendKeyEvent(new KeyEvent(eventTime, eventTime, KeyEvent.ACTION_DOWN, code, 0, 0,
                        KeyCharacterMap.VIRTUAL_KEYBOARD, 0, KeyEvent.FLAG_SOFT_KEYBOARD | KeyEvent.FLAG_KEEP_TOUCH_MODE));
                super.sendKeyEvent(new KeyEvent(SystemClock.uptimeMillis(), eventTime, KeyEvent.ACTION_UP, code, 0, 0,
                        KeyCharacterMap.VIRTUAL_KEYBOARD, 0, KeyEvent.FLAG_SOFT_KEYBOARD | KeyEvent.FLAG_KEEP_TOUCH_MODE));
            }
        };
        return connection;
    }
    public static int getAndroidInputType(int type) {
        int inputType;
        switch (type) {
            case InputCons.NumberPad:
                inputType = InputType.TYPE_CLASS_NUMBER;
                break;
            case InputCons.PhonePad:
                inputType = InputType.TYPE_CLASS_PHONE;
                break;
            case InputCons.Email:
                inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
                break;
            case InputCons.Password:
                inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD;
                break;
            case InputCons.URI:
                inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI;
                break;
            default:
                inputType = InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
                break;
        }
        return inputType;
    }
}
