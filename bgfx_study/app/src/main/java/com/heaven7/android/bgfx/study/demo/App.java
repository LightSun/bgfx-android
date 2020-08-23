package com.heaven7.android.bgfx.study.demo;

import android.app.Application;

import com.heaven7.android.bgfx.study.demo.lua.Luaer;

public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        Luaer.init(this);
    }
}
