package com.heaven7.android.hbmdx.input;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import androidx.annotation.Keep;

import com.heaven7.android.bgfx.study.demo.BgfxLuaView;
import com.heaven7.android.bgfx.study.demo.lua.Luaer;
import com.heaven7.android.hbmdx.HbConfig;
import com.heaven7.core.util.Logger;
import com.heaven7.core.util.MainWorker;
import com.heaven7.java.base.anno.CalledInternal;

import java.lang.ref.WeakReference;
import java.util.List;

public final class AndroidInput {

    private final HbConfig config = new HbConfig();
    private long nativePtr;

    private WeakReference<BgfxLuaView> mWeakView;
    private Context mAppContext;
    private Vibrator mVibrator;
    //sensors
    private SensorManager mSensorManager;
    private SensorListener accelerometerListener;
    private SensorListener gyroscopeListener;
    private SensorListener rotationVectorListener;
    private SensorListener compassListener;

    public boolean onGenericMotion(MotionEvent event) {
        MotionEventWrapper wrapper = MotionEventWrapper.obtain();
        wrapper.setEvent(MotionEvent.obtain(event));
        return nOnGenericMotion(getNativePtr(), wrapper.getNativePtr(), wrapper);
    }

    public void onTouch(MotionEvent event) {
        MotionEventWrapper wrapper = MotionEventWrapper.obtain();
        wrapper.setEvent(MotionEvent.obtain(event));
        nOnTouch(getNativePtr(), wrapper.getNativePtr(), wrapper);
    }
    public boolean onKey(int keyCode, KeyEvent event) {
        KeyEventWrapper wrapper = KeyEventWrapper.obtain();
        wrapper.setEvent(new KeyEvent(event));
        return nOnKey(getNativePtr(), wrapper.getNativePtr(), keyCode, wrapper);
    }
    public void setView(BgfxLuaView view){
        mWeakView = new WeakReference<>(view);
        mAppContext = view.getContext().getApplicationContext();
        mVibrator = (Vibrator) mAppContext.getSystemService(Context.VIBRATOR_SERVICE);
    }
    public void onResume(){
        registerSensorListeners();
    }
    public void onPause(){
        unregisterSensorListeners();
    }
    private void registerSensorListeners () {
        if(mSensorManager == null){
            mSensorManager = (SensorManager)mAppContext.getSystemService(Context.SENSOR_SERVICE);
        }
        SensorManager manager = mSensorManager;
        if(manager == null){
            nSetSensorAvailables(getNativePtr(), false, false, false, false);
        }else {
            boolean accelerometerAvailable = false;
            boolean gyroscopeAvailable = false;
            boolean rotationVectorAvailable = false;
            boolean compassAvailable = false;

            if (config.useAccelerometer) {
                // manager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
                if (manager.getSensorList(Sensor.TYPE_ACCELEROMETER).isEmpty()) {
                    accelerometerAvailable = false;
                } else {
                    Sensor accelerometer = manager.getSensorList(Sensor.TYPE_ACCELEROMETER).get(0);
                    accelerometerListener = new SensorListener();
                    accelerometerAvailable = manager.registerListener(accelerometerListener, accelerometer,
                            config.sensorDelay);
                }
            }

            if (config.useGyroscope) {
                // manager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
                if (manager.getSensorList(Sensor.TYPE_GYROSCOPE).isEmpty()) {
                    gyroscopeAvailable = false;
                } else {
                    Sensor gyroscope = manager.getSensorList(Sensor.TYPE_GYROSCOPE).get(0);
                    gyroscopeListener = new SensorListener();
                    gyroscopeAvailable = manager.registerListener(gyroscopeListener, gyroscope,
                            config.sensorDelay);
                }
            }

            if (config.useRotationVectorSensor){
                // if (manager == null) manager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
                List<Sensor> rotationVectorSensors = manager.getSensorList(Sensor.TYPE_ROTATION_VECTOR);
                if (!rotationVectorSensors.isEmpty()){
                    rotationVectorListener = new SensorListener();
                    for (Sensor sensor : rotationVectorSensors){ // favor AOSP sensor
                        if (sensor.getVendor().equals("Google Inc.") && sensor.getVersion() == 3){
                            rotationVectorAvailable = manager.registerListener(rotationVectorListener, sensor,
                                    config.sensorDelay);
                            break;
                        }
                    }
                    if (!rotationVectorAvailable)
                        rotationVectorAvailable = manager.registerListener(rotationVectorListener, rotationVectorSensors.get(0),
                                config.sensorDelay);
                }
            }

            if (config.useCompass && !rotationVectorAvailable) {
                //if (manager == null) manager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
                Sensor sensor = manager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
                if (sensor != null) {
                    compassAvailable = accelerometerAvailable;
                    if (compassAvailable) {
                        compassListener = new SensorListener();
                        compassAvailable = manager.registerListener(compassListener, sensor, config.sensorDelay);
                    }
                }
            }
            nSetSensorAvailables(getNativePtr(), accelerometerAvailable, gyroscopeAvailable,
                    rotationVectorAvailable, compassAvailable);
        }
        Logger.d("AndroidInput", "sensor listener setup");
    }
    private void unregisterSensorListeners() {
        SensorManager manager = this.mSensorManager;
        if (manager != null) {
            if (accelerometerListener != null) {
                manager.unregisterListener(accelerometerListener);
                accelerometerListener = null;
            }
            if (gyroscopeListener != null) {
                manager.unregisterListener(gyroscopeListener);
                gyroscopeListener = null;
            }
            if (rotationVectorListener != null) {
                manager.unregisterListener(rotationVectorListener);
                rotationVectorListener = null;
            }
            if (compassListener != null) {
                manager.unregisterListener(compassListener);
                compassListener = null;
            }
            manager = null;
        }
        Logger.d("AndroidInput", "sensor listener tear down");
    }
    @CalledInternal
    @Keep //native
    private static void setOnscreenKeyboardVisible(final AndroidInput input, final boolean visible, final int type) {
        MainWorker.post(new Runnable() {
            public void run () {
                BgfxLuaView luaView = input.mWeakView.get();
                if(luaView == null){
                    return;
                }
                InputMethodManager manager = (InputMethodManager)input.mAppContext.getSystemService(Context.INPUT_METHOD_SERVICE);
                if (visible) {
                    if(luaView.getOnscreenKeyboardType() != type) {
                        luaView.setOnscreenKeyboardType(type);
                        manager.restartInput(luaView);
                    }
                    luaView.setFocusable(true);
                    luaView.setFocusableInTouchMode(true);
                    manager.showSoftInput(luaView, 0);
                } else {
                    manager.hideSoftInputFromWindow(luaView.getWindowToken(), 0);
                }
            }
        });
    }
    @Keep
    private static void getRotationMatrixFromVector(float[] r, float[] rotationVectorValues){
        SensorManager.getRotationMatrixFromVector(r, rotationVectorValues);
    }
    @Keep
    private static boolean getRotationMatrix(float[] R, float[] I, float[] gravity, float[] geomagnetic){
        return SensorManager.getRotationMatrix(R, I, gravity, geomagnetic);
    }
    @Keep
    private static void getOrientation(float[] R, float[] orientation){
        SensorManager.getOrientation(R, orientation);
    }
    @Keep
    private static int getRotation(AndroidInput input) {
        BgfxLuaView luaView = input.mWeakView.get();
        if(luaView == null){
            return 0;
        }
        Context context = luaView.getContext();
        int orientation = 0;
        if (context instanceof Activity) {
            orientation = ((Activity)context).getWindowManager().getDefaultDisplay().getRotation();
        } else {
            orientation = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
        }
        switch (orientation) {
            case Surface.ROTATION_0:
                return 0;
            case Surface.ROTATION_90:
                return 90;
            case Surface.ROTATION_180:
                return 180;
            case Surface.ROTATION_270:
                return 270;
            default:
                return 0;
        }
    }
    @Keep //native
    public void vibrate(int milliseconds) {
        if(mVibrator == null){
            return;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            mVibrator.vibrate(VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE));
        else
            mVibrator.vibrate(milliseconds);
    }
    @Keep //native
    public void vibrate(long[] pattern, int repeat) {
        if(mVibrator != null){
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                mVibrator.vibrate(VibrationEffect.createWaveform(pattern, repeat));
            else
                mVibrator.vibrate(pattern, repeat);
        }
    }
    @Keep //native
    public void cancelVibrate(){
        mVibrator.cancel();
    }

    public AndroidInput() {
        nativePtr = nAlloc(this);
    }

    public long getNativePtr() {
        return nativePtr;
    }
    @Override
    protected void finalize() throws Throwable {
        if (nativePtr != 0) {
            nDealloc(nativePtr);
            nativePtr = 0;
        }
        super.finalize();
    }
    /** Our implementation of SensorEventListener. Because Android doesn't like it when we register more than one Sensor to a single
     * SensorEventListener, we add one of these for each Sensor. Could use an anonymous class, but I don't see any harm in
     * explicitly defining it here. Correct me if I am wrong. */
    private class SensorListener implements SensorEventListener {
        @Override
        public void onAccuracyChanged (Sensor arg0, int arg1) {
        }
        @Override
        public void onSensorChanged (SensorEvent event) {
            switch (event.sensor.getType()){
                case Sensor.TYPE_ACCELEROMETER:
                    nAccelerometer_Changed(getNativePtr(), event.values);
                break;

                case Sensor.TYPE_MAGNETIC_FIELD:
                    nMagnetic_field_Changed(getNativePtr(), event.values);
                    break;

                case Sensor.TYPE_GYROSCOPE:
                    nGyroscope_Changed(getNativePtr(), event.values);
                    break;

                case Sensor.TYPE_ROTATION_VECTOR:
                    nRotation_vector_Changed(getNativePtr(), event.values);
                    break;
            }
        }
    }
    private static native long nAlloc(AndroidInput input);
    private static native void nDealloc(long ptr);
    private static native void nOnTouch(long inputPtr, long mePtr, MotionEventWrapper wrapper);
    private static native boolean nOnGenericMotion(long inputPtr, long mePtr, MotionEventWrapper wrapper);
    private static native boolean nOnKey(long inputPtr, long kePtr, int keyCode, KeyEventWrapper wrapper);

    private static native void nSetSensorAvailables(long ptr, boolean accelerometer, boolean gyroscope,
                                                    boolean rotationVector,boolean compass);
    private static native void nAccelerometer_Changed(long ptr,float[] values);
    private static native void nMagnetic_field_Changed(long ptr,float[] values);
    private static native void nGyroscope_Changed(long ptr,float[] values);
    private static native void nRotation_vector_Changed(long ptr,float[] values);
}

