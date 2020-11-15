package com.heaven7.android.bgfx.study.demo.lua;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.os.Environment;

import com.heaven7.android.bgfx.study.demo.NativeApi;
import com.heaven7.core.util.Logger;
import com.heaven7.java.base.util.FileUtils;
import com.heaven7.java.base.util.IOUtils;
import com.heaven7.java.lua.LuaSearcher;
import com.heaven7.java.lua.LuaState;
import com.heaven7.java.lua.LuaWrapper;
import com.heaven7.java.pc.schedulers.Schedulers;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.Reader;
import java.util.Arrays;
import java.util.List;

/**
 * Created by heaven7 on 2019/8/13.
 */
public final class Luaer {

    private static final String TAG = "Luaer";
    private static final String LUA_DIR = Environment.getExternalStorageDirectory() + "/vida/lua";
    private static final String LUA_PARENT_DIR = Environment.getExternalStorageDirectory() + "/vida";

    private final Context context;
    private LuaState mLuaState;

    private static Luaer sInstance;

    private Luaer(Context context) {
        this.context = context;
    }

    public static synchronized void init(Context context){
        if(sInstance == null){
            sInstance = new Luaer(context);
            sInstance.initLuaState();
            sInstance.initEnv(true);

            NativeApi.initLuaBgfx(sInstance.mLuaState.getNativePointer());
        }
    }

    public static Luaer get(){
        return sInstance;
    }

    public void initLuaState(){
        if(mLuaState == null){
            mLuaState = new LuaState();
            //init LuaWrapper
            LuaWrapper.getDefault();
            // AndroidEnv.initialize(context, this);
        }
    }
    public void destroyLuaState(){
        if(mLuaState != null){
            mLuaState.destroyNative();
            mLuaState = null;
        }
    }
    public LuaState getLuaState(){
        return mLuaState;
    }
    public long getLuaPtr(){
        return mLuaState.getNativePointer();
    }

    public Context getApplicationContext(){
        return context.getApplicationContext();
    }
    public File getFilesDir(){
        return context.getFilesDir();
    }
    public AssetManager getAssets(){
        return context.getAssets();
    }
    public Resources getResources(){
        return context.getResources();
    }

    public String doFile(String relativePath){
        String path = LUA_DIR + "/" + relativePath;
        if(!new File(path).exists()){
            return "can't find lua file,  path = " + path;
        }
        if(mLuaState.doFile(path) != 0){
            String msg = mLuaState.toString(-1);
            mLuaState.pop(1);
            return msg;
        }
        return null;
    }
    public String loadFile(String relativePath){
        String path = LUA_DIR + "/" + relativePath;
        if(!new File(path).exists()){
            return "can't find lua file,  path = " + path;
        }
        if(mLuaState.loadFile(path) != 0){
            String msg = mLuaState.toString(-1);
            mLuaState.pop(1);
            return msg;
        }
        return null;
    }

    public void initEnv(final boolean force){
        FileUtils.deleteDir(new File(LUA_DIR));

        final List<String> clibs = Arrays.asList();
        LuaWrapper.getDefault().registerLuaSearcher(new LuaSearcher() {
            @Override
            public String getLuaFilepath(String module) {
                if(clibs.contains(module)){
                    return null;
                }
                if(module.contains(".")){
                    module = module.replaceAll("\\.", "/");
                }
                Logger.d(TAG, "getLuaFilepath", "module = " + module);
                return LUA_DIR + "/" + module + ".lua";
            }
            @Override
            public String getClibFilepath(String module) {
                Logger.d(TAG, "getClibFilepath", "module = " + module);
                //return LUA_DIR + "/lib" + module + ".so";
                return new File(getFilesDir(), "lib"+ module + ".so").getPath();
            }
        });
        Schedulers.io().newWorker().schedule(new Runnable() {
            @Override
            public void run() {
                AssetsUtils.copyAll(getApplicationContext(), "lua", LUA_PARENT_DIR);
                for (String clib : clibs){
                    String libname = "lib" + clib + ".so";
                    File dst = new File(getFilesDir(), libname);
                    System.out.println( libname + ": path is " + dst.getPath());
                    if(dst.exists()){
                        if(!force){
                            System.out.println(libname + ": already copied.");
                            return;
                        }else {
                            dst.delete();
                        }
                    }
                    //lua load libcjson .the c json can't be load on sdcard for high-version android
                    try {
                        InputStream in = getAssets().open("clua/" + libname);
                        OutputStream out = new FileOutputStream(dst);
                        IOUtils.copyLarge(in, out);
                        IOUtils.closeQuietly(in);
                        IOUtils.closeQuietly(out);
                        System.out.println(libname + " copy ok.");
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                Logger.d(TAG, "run", "lua script copy done");
            }
        });
    }

    public static String readStringWithLine(Reader r) throws IOException {
        BufferedReader br = r instanceof BufferedReader ? (BufferedReader) r : new BufferedReader(r);

        String str;
        StringBuilder sb = new StringBuilder();
        while ((str = br.readLine()) != null) {
            sb.append(str).append("\n");
        }
        return sb.toString();
    }
    public void loadLuaAssets(String file) {
        InputStreamReader in = null;
        try {
            in = new InputStreamReader(getAssets().open(file));
            String str = Luaer.readStringWithLine(in);
           // System.out.println("loadLuaAssets: \n" + str);
            int state = mLuaState.doString(str);
            Logger.i(TAG, "loadLua", "state = " + state + ", " + mLuaState.toString(-1));
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeQuietly(in);
        }
    }
    public String loadLuaAssetsAsString(String file) {
        InputStreamReader in = null;
        try {
            in = new InputStreamReader(getAssets().open(file));
            return Luaer.readStringWithLine(in);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            IOUtils.closeQuietly(in);
        }
    }
    public void loadLuaRaw(int file) {
        InputStreamReader in = null;
        try {
            in = new InputStreamReader(getResources().openRawResource(file));
            int state = mLuaState.doString(Luaer.readStringWithLine(in));
            String msg = mLuaState.toString(-1);
            Logger.i(TAG, "loadLua", "state = " + state + " ,msg = " + msg);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeQuietly(in);
        }
    }
}
