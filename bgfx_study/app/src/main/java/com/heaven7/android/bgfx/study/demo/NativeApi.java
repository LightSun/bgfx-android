package com.heaven7.android.bgfx.study.demo;

public final class NativeApi {

    //set use lua or not.
    static native void setUseLua(boolean useLua);
    static native void destroyAll();

    static native void initAssets(Object ctx, Object assetM);
    static native void initializeSurface(Object src, Object surface, long luaPtr);

    static native void destroySurface(Object src);
}
