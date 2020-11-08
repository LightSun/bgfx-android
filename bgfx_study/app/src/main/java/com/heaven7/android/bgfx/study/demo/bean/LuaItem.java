package com.heaven7.android.bgfx.study.demo.bean;

import com.heaven7.adapter.BaseSelector;

public class LuaItem extends BaseSelector {
    public final String path;
    public final String desc;
    public final boolean needSurface;

    public LuaItem(String path, String desc, boolean needSurface) {
        this.path = path;
        this.desc = desc;
        this.needSurface = needSurface;
    }

    public LuaItem(String path, String desc) {
        this(path, desc, false);
    }
}