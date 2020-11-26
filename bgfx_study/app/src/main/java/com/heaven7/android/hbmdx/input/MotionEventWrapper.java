package com.heaven7.android.hbmdx.input;

import android.view.MotionEvent;

import androidx.annotation.Keep;

import com.heaven7.java.base.anno.CalledInternal;

import java.util.LinkedList;

@Keep
public final class MotionEventWrapper {

    private MotionEvent event;
    private long nativePtr;

    private static final LinkedList<MotionEventWrapper> sPool = new LinkedList<>();

    static {
        for (int i = 0; i < InputCons.POOL_COUNT_MOTION; i++) {
            sPool.addLast(new MotionEventWrapper());
        }
    }

    public static MotionEventWrapper obtain(){
        MotionEventWrapper wrapper = sPool.pollFirst();
        return wrapper != null ? wrapper : new MotionEventWrapper();
    }
    @Keep
    public static void recycle(MotionEventWrapper wrapper){
        wrapper.recycleEvent();
        sPool.addLast(wrapper);
    }

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
        int buttonState = event.getButtonState();
        float pressure = event.getPressure(pointerIndex);
        nSet(nativePtr, action, x, y, pointerIndex, pointerId, pointerCount, buttonState, pressure);
    }

    @Keep
    @CalledInternal
    public static void getInfo(MotionEventWrapper wrapper, int pointerIndex, float[] out){
        MotionEvent event = wrapper.event;
        if(event != null){
            out[0] = event.getPointerId(pointerIndex);
            out[1] = event.getX(pointerIndex);
            out[2] = event.getY(pointerIndex);
            out[3] = event.getPressure(pointerIndex);
        }else {
            out[0] = -1;
            out[1] = 0;
            out[2] = 0;
            out[3] = 0;
        }
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
        recycleEvent();
        super.finalize();
    }
    private void recycleEvent(){
        if(event != null){
            event.recycle();
            event = null;
        }
    }

    private static native long nAlloc();

    private static native void nDealloc(long ptr);

    private static native void nSet(long ptr, int action, int x, int y,
                                    int pointerIndex, int pointerId, int pointerCount,
                                    int buttonState, float pressure);
}

