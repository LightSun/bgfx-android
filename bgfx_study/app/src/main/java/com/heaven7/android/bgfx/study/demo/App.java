package com.heaven7.android.bgfx.study.demo;

import android.app.Application;

import com.heaven7.android.bgfx.study.demo.lua.Luaer;
import com.heaven7.android.hbmdx.Heaven7;

public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        Luaer.init(this);

        Heaven7.init(getApplicationContext(), getAssets());
    }

    @Override
    public void onTerminate() {
        Heaven7.destroy();
        super.onTerminate();
    }
}
