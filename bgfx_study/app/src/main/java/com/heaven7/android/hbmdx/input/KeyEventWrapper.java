package com.heaven7.android.hbmdx.input;

import android.view.KeyEvent;

public class KeyEventWrapper {

    private KeyEvent event;
    private long nativePtr;

    public KeyEvent getEvent() {
        return event;
    }
    public void setEvent(KeyEvent event) {
        this.event = event;
    }

    public KeyEventWrapper() {
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
