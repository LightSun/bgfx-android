package com.heaven7.android.bgfx.study.demo.lua;

import android.content.Context;

import com.heaven7.java.base.util.IOUtils;
import com.heaven7.java.base.util.Predicates;
import com.heaven7.java.base.util.ResourceLoader;
import com.heaven7.java.visitor.MapFireVisitor;
import com.heaven7.java.visitor.ResultVisitor;
import com.heaven7.java.visitor.collection.KeyValuePair;
import com.heaven7.java.visitor.collection.VisitServices;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * the file manager of assets
 * Created by heaven7 on 2018/10/27 0027.
 */
public class AssetsFileCopyUtils {

    /**
     *  copy the all files from asset dir
     * @param context the context
     * @param assetParentDir the asset dir
     * @param dstParentDir the dest parent dir
     * @return the copied files
     */
    public static List<String> copyAll(final Context context, final String assetParentDir, final String dstParentDir){
        List<String> files = new ArrayList<>();
        try {
            getAssetFilesFromDir(context, assetParentDir, files);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
       return VisitServices.from(files).map2mapAsKey(new ResultVisitor<String, String>() {
           @Override
           public String visit(String s, Object param) {
               return dstParentDir + "/" + s;
           }
       }).fire(new MapFireVisitor<String, String>() {
           @Override
           public Boolean visit(KeyValuePair<String, String> pair, Object param) {
               copy(context, pair.getKey(), pair.getValue(), false);
               return null;
           }
       }).mapValue().getAsList();
    }
    public static void copy(Context context, String assetPath, String dstFile, boolean force){
        File outFile = new File(dstFile);
        if(outFile.exists()){
            if(!force && outFile.isFile()){
                return;
            }else{
                outFile.delete();
            }
        }else{
            File parentFile = outFile.getParentFile();
            if(parentFile != null) {
                parentFile.mkdirs();
            }
        }
        InputStream in = null;
        OutputStream out = null;
        try {
            in = ResourceLoader.getDefault().loadFileAsStream(context, assetPath);
            out = new FileOutputStream(outFile);
            IOUtils.copyLarge(in, out);
        }catch (IOException e){
            throw new RuntimeException(e);
        }finally {
            IOUtils.closeQuietly(in);
            IOUtils.closeQuietly(out);
        }
    }
    public static void copy(Context context, AssetItem item, boolean force){
        copy(context, item.getAssetPath(), item.getDstFile(), force);
    }

    private static void getAssetFilesFromDir(Context context, String dir, List<String> outPaths) throws IOException {
        String[] children = context.getAssets().list(dir);
        if(!Predicates.isEmpty(children)){
            for(String path : children){
                String fullPath = dir + "/" + path;
                if(path.contains(".")){
                    //file
                    outPaths.add(fullPath);
                }else{
                    //dir
                    getAssetFilesFromDir(context, fullPath, outPaths);
                }
            }
        }
    }

    static class AssetItem{
        private String assetPath;
        private String dstFile;

        public String getAssetPath() {
            return assetPath;
        }
        public void setAssetPath(String assetPath) {
            this.assetPath = assetPath;
        }
        public String getDstFile() {
            return dstFile;
        }
        public void setDstFile(String dstFile) {
            this.dstFile = dstFile;
        }
    }
}
