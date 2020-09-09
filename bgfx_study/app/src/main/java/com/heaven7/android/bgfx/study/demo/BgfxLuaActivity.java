package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
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

    private Luaer mLuaer = Luaer.get();
    private List<Item> mItems;

    @Override
    protected int getLayoutId() {
        return R.layout.ac_bgfx_lua_demo;
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        NativeApi.setUseLua(true);
        super.onCreate(savedInstanceState);

        mRv = findViewById(R.id.rv);
        mBgfxLuaView = findViewById(R.id.bgfx_lua_view);

        mItems = createItems();
        mRv.setLayoutManager(new LinearLayoutManager(this));
        mRv.setAdapter(new QuickRecycleViewAdapter<Item>(R.layout.item_name, mItems) {
            @Override
            protected void onBindData(Context context, int position, final Item item, int itemLayoutId, ViewHelper2 helper) {
                helper.setText(android.R.id.text1, item.desc)
                        .setRootOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                mRv.setVisibility(View.GONE);
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

    public void onClickSwitchDemo(View view){
        mRv.setVisibility(View.VISIBLE);
    }

    @Override
    protected void onDestroy() {
        NativeApi.setUseLua(false);
        super.onDestroy();
    }

    private List<Item> createItems() {
        List<Item> items = new ArrayList<>();
        items.add(new Item("lua/curbes.lua", "Curbes"));
        items.add(new Item("lua/hello_world.lua", "Hello World"));
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
