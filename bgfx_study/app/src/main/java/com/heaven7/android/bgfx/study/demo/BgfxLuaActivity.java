package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.lifecycle.Lifecycle;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.heaven7.adapter.QuickRecycleViewAdapter;
import com.heaven7.adapter.util.ViewHelper2;
import com.heaven7.android.bgfx.study.demo.bean.LuaItem;
import com.heaven7.android.bgfx.study.demo.lua.Luaer;

import java.util.ArrayList;
import java.util.List;

public class BgfxLuaActivity extends BgfxDemoActivity {

    BgfxLuaView mBgfxLuaView;
    RecyclerView mRv;

    private Luaer mLuaer = Luaer.get();
    private List<LuaItem> mLuaItems;

    @Override
    protected int getLayoutId() {
        return R.layout.ac_bgfx_lua_demo;
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        NativeApi.setUseLua(true);
        super.onCreate(savedInstanceState);

        String path = getIntent().getStringExtra(Constants.KEY_LUA_FILE);
        mRv = findViewById(R.id.rv);
        mBgfxLuaView = findViewById(R.id.bgfx_lua_view);

        mLuaItems = createItems();
        mRv.setLayoutManager(new LinearLayoutManager(this));
        mRv.setAdapter(new QuickRecycleViewAdapter<LuaItem>(R.layout.item_name, mLuaItems) {
            @Override
            protected void onBindData(Context context, int position, final LuaItem luaItem, int itemLayoutId, ViewHelper2 helper) {
                helper.setText(android.R.id.text1, luaItem.desc)
                        .setRootOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                mRv.setVisibility(View.GONE);
                                mBgfxLuaView.setScriptFile(mLuaer, luaItem.path);
                            }
                        });
            }
        });
        //run the path
        final String runPath = path != null ? path : mLuaItems.get(0).path;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mBgfxLuaView.setScriptFile(mLuaer, runPath);
            }
        });
    }

    public void onClickSwitchDemo(View view){
        mRv.setVisibility(View.VISIBLE);
        //startActivity(new );
    }

    @Override
    protected void onPause() {
        NativeApi.onLifeCycle(mLuaer.getLuaPtr(), Lifecycle.Event.ON_PAUSE.ordinal());
        super.onPause();
    }
    @Override
    protected void onResume() {
        super.onResume();
        mBgfxLuaView.addPendingTask(new Runnable() {
            @Override
            public void run() {
                NativeApi.onLifeCycle(mLuaer.getLuaPtr(), Lifecycle.Event.ON_RESUME.ordinal());
            }
        });
    }
    @Override
    protected void onDestroy() {
        NativeApi.setUseLua(false);
        NativeApi.onLifeCycle(mLuaer.getLuaPtr(), Lifecycle.Event.ON_DESTROY.ordinal());
        super.onDestroy();
    }

    private List<LuaItem> createItems() {
        List<LuaItem> luaItems = new ArrayList<>();
        luaItems.add(new LuaItem("lua/curbes.lua", "Curbes"));
        luaItems.add(new LuaItem("lua/hello_world.lua", "Hello World"));
        return luaItems;
    }

}
