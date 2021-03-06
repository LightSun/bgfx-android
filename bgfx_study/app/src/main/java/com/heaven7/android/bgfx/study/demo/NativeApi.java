package com.heaven7.android.bgfx.study.demo;

import android.os.Handler;
import android.os.Looper;

import androidx.annotation.Keep;

import com.heaven7.core.util.MainWorker;
import com.heaven7.java.lua.LuaState;

public final class NativeApi {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("bgfx_core");
        System.loadLibrary("bgfx_study");
    }

    //set use lua or not.
    public static native void setUseLua(boolean useLua);
    public static native void destroyAll();

    public static native void initAssets(Object ctx, Object assetM);
    public static native void initializeSurface(Object src, Object surface, long luaPtr);

    public static native void destroySurface(Object src);

    public native static void initLuaBgfx(long luaPtr);
    //1: func_error(string)
    //2: func_result(...)
    //3: func() -> ...
    //4: ... : parameters.
    //5: count of parameters
    //6: count of func result
    @Keep
    static void postMain(final long luaStatePtr){
        MainWorker.post(new Runnable() {
            @Override
            public void run() {
                final LuaState state = new LuaState(luaStatePtr);
                int nResult = (int) state.toLong(-1);
                int nArg = (int) state.toLong(-2);
                state.pop(2);
                System.out.println("---------- postMain -------");
                state.dumpLuaStack();
                String error = state.pcallM(nArg, nResult, 0);//TODO crashed .why ???
                state.dumpLuaStack();
                System.out.println("---------- postMain -------");
                if(error != null){
                    //func_err, func_result, msg
                    state.pushValue(-3);  //func_err, func_result, msg, func_err
                    state.pushValue(-2);  //func_err, func_result, msg, func_err, msg
                    String m = state.pcallM(1, 0, 0);
                    if(m != null){
                        throw new RuntimeException("callback for 'func_error' should never failed. error = " + m);
                    }
                    state.pop(1);  //func_err, func_result
                }else {
                    //func_err, func_result, ...
                    String m = state.pcallM(nResult, 0, 0);
                    if(m != null){
                        System.err.println("callback for 'func_result' should never failed. error = " + m);
                    }
                }
            }
        });
    }

}
