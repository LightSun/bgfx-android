package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.RecyclerView;

import com.heaven7.adapter.BaseSelector;
import com.heaven7.adapter.QuickRecycleViewAdapter;
import com.heaven7.adapter.util.ViewHelper2;
import com.heaven7.android.bgfx.study.demo.lua.Luaer;

import java.util.ArrayList;
import java.util.List;

public class BgfxLuaActivity extends BgfxDemoActivity {

    BgfxLuaView mBgfxLuaView;
    RecyclerView mRv;

    private Luaer mLuaer;
    private List<Item> mItems;

    @Override
    protected int getLayoutId() {
        return R.layout.ac_bgfx_lua_demo;
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mLuaer = new Luaer(this);
        mLuaer.initLuaState();

        mRv = findViewById(R.id.rv);
        mBgfxLuaView = findViewById(R.id.bgfx_lua_view);

        mItems = createItems();
        mRv.setAdapter(new QuickRecycleViewAdapter<Item>(R.layout.item_name, mItems) {
            @Override
            protected void onBindData(Context context, int position, final Item item, int itemLayoutId, ViewHelper2 helper) {
                helper.setText(android.R.id.text1, item.desc)
                        .setRootOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                mBgfxLuaView.setScriptFile(mLuaer, item.path);
                            }
                        });
            }
        });
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mBgfxLuaView.setScriptFile(mLuaer, mItems.get(0).path);
            }
        });
    }

    private List<Item> createItems() {
        List<Item> items = new ArrayList<>();
        items.add(new Item("lua/hello_world", "Hello World"));
        items.add(new Item("lua/curbes", "Curbes"));
        return items;
    }

    public static class Item extends BaseSelector {
        final String path;
        final String desc;

        public Item(String path, String desc) {
            this.path = path;
            this.desc = desc;
        }
    }
}
