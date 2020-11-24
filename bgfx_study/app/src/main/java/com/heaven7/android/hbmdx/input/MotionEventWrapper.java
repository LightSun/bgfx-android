package com.heaven7.android.hbmdx.input;

import android.view.MotionEvent;

public final class MotionEventWrapper {

    private MotionEvent event;
    private long nativePtr;

    public MotionEvent getEvent() {
        return event;
    }

    public void setEvent(MotionEvent event) {
        this.event = event;

        final int action = event.getAction() & MotionEvent.ACTION_MASK;
        int pointerIndex = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
        int pointerId = event.getPointerId(pointerIndex);
        int pointerCount = event.getPointerCount();

        int x = (int) event.getX(pointerIndex);
        int y = (int) event.getY(pointerIndex);
        long timeStamp = System.nanoTime();
        int buttonState = event.getButtonState();
        float pressure = event.getPressure(pointerIndex);
        nSet(nativePtr, action, x, y, pointerIndex, pointerId, pointerCount, buttonState, pressure, timeStamp);
    }

    public MotionEventWrapper() {
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

    private static native void nSet(long ptr, int action, int x, int y,
                                    int pointerIndex, int pointerId, int pointerCount,
                                    int buttonState, float pressure, long timeStamp);
}

