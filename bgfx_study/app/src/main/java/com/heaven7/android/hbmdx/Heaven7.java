package com.heaven7.android.hbmdx;

import android.content.Context;
import android.content.res.AssetManager;

import com.heaven7.android.bgfx.study.demo.lua.Luaer;

public final class Heaven7 {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("bgfx_core");
        System.loadLibrary("bgfx_study");
    }

    public static void init(Context context, AssetManager am){
        nInit(context, am, Luaer.get().getLuaPtr());
    }
    public static void destroy(){
        nDestroy();
    }

    private static native void nInit(Context context, AssetManager am, long luaPtr);
    private static native void nDestroy();
}
