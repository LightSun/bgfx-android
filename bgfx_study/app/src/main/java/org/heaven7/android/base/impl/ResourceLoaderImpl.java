package org.heaven7.android.base.impl;

import android.content.Context;

import androidx.annotation.Keep;

import com.heaven7.java.base.util.ResourceLoader;
import com.heaven7.java.base.util.TextUtils;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

@Keep
public class ResourceLoaderImpl extends ResourceLoader {

    @Override
    public InputStream loadFileAsStream(Object ctx, String path) throws IOException {
        if (!TextUtils.isRelativePath(path)) {
            return new FileInputStream(path);
        }
        if(ctx instanceof Context){
            Context con = (Context) ctx;
            return con.getAssets().open(path);
        }
        throw new UnsupportedOperationException("wrong context. ctx is " + ctx.getClass().getName());
    }
}