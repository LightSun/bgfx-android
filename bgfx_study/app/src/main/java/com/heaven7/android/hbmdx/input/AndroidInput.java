package com.heaven7.android.hbmdx.input;

import android.view.MotionEvent;

public final class AndroidInput {

    private long nativePtr;

    public void onTouch(MotionEvent event) {
        MotionEventWrapper wrapper = MotionEventWrapper.obtain();
        wrapper.setEvent(event);
        nOnTouch(getNativePtr(), wrapper.getNativePtr(), wrapper);
    }

    private static native void nOnTouch(long inputPtr, long mePtr, MotionEventWrapper wrapper);

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
}
