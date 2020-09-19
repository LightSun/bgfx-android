package com.heaven7.android.bgfx.study.demo;

import android.content.Context;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.heaven7.adapter.QuickRecycleViewAdapter;
import com.heaven7.adapter.util.ViewHelper2;
import com.heaven7.android.bgfx.study.demo.lua.Luaer;
import com.heaven7.core.util.Logger;

import java.util.ArrayList;
import java.util.List;

public class LuaUnitTestsAc extends AppCompatActivity {

    private static final String TAG = "LuaUnitTestsAc";
    RecyclerView mRv;

    private Luaer mLuaer = Luaer.get();
    private List<BgfxLuaActivity.Item> mItems;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ac_rv);

        mRv = findViewById(R.id.rv);
        mRv.setLayoutManager(new LinearLayoutManager(this));

        mItems = createItems();
        mRv.setAdapter(new QuickRecycleViewAdapter<BgfxLuaActivity.Item>(R.layout.item_name, mItems) {
            @Override
            protected void onBindData(Context context, int position, final BgfxLuaActivity.Item item, int itemLayoutId, ViewHelper2 helper) {
                helper.setText(android.R.id.text1, item.desc)
                        .setRootOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(View v) {
                                runTest(item);
                            }
                        });
            }
        });
    }

    private List<BgfxLuaActivity.Item> createItems() {
        List<BgfxLuaActivity.Item> items = new ArrayList<>();
        items.add(new BgfxLuaActivity.Item("lua/tests/test_int64.lua", "test int64"));
        items.add(new BgfxLuaActivity.Item("lua/tests/lua54.lua", "test lua5.4"));
        items.add(new BgfxLuaActivity.Item("lua/tests/memory_test.lua", "test memory"));
        items.add(new BgfxLuaActivity.Item("lua/tests/memory_mat.lua", "test memory mat"));
        items.add(new BgfxLuaActivity.Item("lua/tests/memory_any.lua", "test memory any"));
        items.add(new BgfxLuaActivity.Item("lua/tests/memory_mul.lua", "test memory mul"));
        return items;
    }
    public void runTest(BgfxLuaActivity.Item item){
        Logger.d(TAG, "runTest", "start run : " + item.path);
        mLuaer.loadLuaAssets(item.path);
    }
}
