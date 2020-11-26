package com.heaven7.android.hbmdx.input;

import android.view.KeyEvent;

import java.util.LinkedList;

public final class KeyEventWrapper {

    private KeyEvent event;
    private long nativePtr;

    private static final LinkedList<KeyEventWrapper> sPool = new LinkedList<>();
    static {
        for (int i = 0; i < InputCons.POOL_COUNT_KEY; i++) {
            sPool.addLast(new KeyEventWrapper());
        }
    }
    public static KeyEventWrapper obtain(){
        KeyEventWrapper wrapper = sPool.pollFirst();
        return wrapper != null ? wrapper : new KeyEventWrapper();
    }
    public static void recycle(KeyEventWrapper wrapper){
        sPool.addLast(wrapper);
    }

    public KeyEvent getEvent() {
        return event;
    }
    public void setEvent(KeyEvent event) {
        this.event = event;
        //set native
        int action = event.getAction();
        int repeatCount = event.getRepeatCount();
        int keyCode = event.getKeyCode();
        String chars = event.getCharacters();
        int unicodeChar = event.getUnicodeChar();
        boolean altPressed = event.isAltPressed();
        nSet(getNativePtr(), action, repeatCount, keyCode, chars, unicodeChar, altPressed);
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
    private static native void nSet(long ptr, int action, int repeatCount, int keyCode, String chars, int unicodeChar, boolean altPressed);
}
