package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.heaven7.adapter.QuickRecycleViewAdapter;
import com.heaven7.adapter.util.ViewHelper2;
import com.heaven7.android.bgfx.study.demo.bean.LuaItem;
import com.heaven7.android.bgfx.study.demo.lua.Luaer;
import com.heaven7.android.util2.LauncherIntent;
import com.heaven7.core.util.Logger;

import java.util.ArrayList;
import java.util.List;

public class LuaUnitTestsAc extends AppCompatActivity {

    private static final String TAG = "LuaUnitTestsAc";
    RecyclerView mRv;

    private Luaer mLuaer = Luaer.get();
    private List<LuaItem> mLuaItems;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ac_rv);

        mRv = findViewById(R.id.rv);
        mRv.setLayoutManager(new LinearLayoutManager(this));

        mLuaItems = createItems();
        mRv.setAdapter(new QuickRecycleViewAdapter<LuaItem>(R.layout.item_name, mLuaItems) {
            @Override
            protected void onBindData(Context context, int position, final LuaItem luaItem, int itemLayoutId, ViewHelper2 helper) {
                helper.setText(android.R.id.text1, luaItem.desc)
                        .setRootOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                runTest(luaItem);
                            }
                        });
            }
        });
    }

    private List<LuaItem> createItems() {
        List<LuaItem> luaItems = new ArrayList<>();
        luaItems.add(new LuaItem("lua/tests/internal_tests.lua", "internal tests"));
        luaItems.add(new LuaItem("lua/tests/test_int64.lua", "test int64"));
        luaItems.add(new LuaItem("lua/tests/lua54.lua", "test lua5.4"));
        luaItems.add(new LuaItem("lua/tests/memory_test.lua", "test memory"));
        luaItems.add(new LuaItem("lua/tests/memory_mat.lua", "test memory mat"));
        luaItems.add(new LuaItem("lua/tests/memory_any.lua", "test memory any"));
        luaItems.add(new LuaItem("lua/tests/memory_mul.lua", "test memory mul"));
        luaItems.add(new LuaItem("lua/tests/mat.lua", "test mat"));
        luaItems.add(new LuaItem("lua/tests/skm.lua", "test sk mem"));
        luaItems.add(new LuaItem("lua/tests/skanym.lua", "test skany mem"));
        luaItems.add(new LuaItem("lua/tests/fr/TestView.lua", "test TestView(canvas)", true));
        return luaItems;
    }
    public void runTest(LuaItem luaItem){
        Logger.d(TAG, "runTest", "start run : " + luaItem.path);
        if(luaItem.needSurface){
            new LauncherIntent.Builder()
                    .setClass(this, BgfxLuaActivity.class)
                    .putExtra(Constants.KEY_LUA_FILE, luaItem.path)
                    .build()
                    .startActivity();
        }else {
            mLuaer.loadLuaAssets(luaItem.path);
        }
    }
}
