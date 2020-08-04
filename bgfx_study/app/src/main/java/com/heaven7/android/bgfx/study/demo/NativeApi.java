package com.heaven7.android.bgfx.study.demo;

public final class NativeApi {

    static native void initAssets(Object ctx, Object assetM);
    static native void initializeSurface(Object src, Object surface);

    static native void destroySurface(Object src, Object surface);

    static native boolean update();
}
