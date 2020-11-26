package com.heaven7.android.hbmdx;

import android.hardware.SensorManager;

public final class HbConfig {

    /** whether to use the accelerometer. default: true **/
    public boolean useAccelerometer = true;

    /** whether to use the gyroscope. default: false **/
    public boolean useGyroscope = false;

    /** Whether to use the compass. The compass enables Input#getRotationMatrix(float[]), Input#getAzimuth(),
     * Input#getPitch(), and Input#getRoll() if {@link #useAccelerometer} is also true.
     * <p>
     * If {@link #useRotationVectorSensor} is true and the rotation vector sensor is available, the compass will not be used.
     * <p>
     * Default: true **/
    public boolean useCompass = true;

    /** Whether to use Android's rotation vector software sensor, which provides cleaner data than that of {@link #useCompass} for
     * nput#getRotationMatrix(float[]), Input#getAzimuth(), Input#getPitch(), and Input#getRoll().
     * The rotation vector sensor uses a combination of physical sensors, and it pre-filters and smoothes the data. If true,
     * {@link #useAccelerometer} is not required to enable rotation data.
     * <p>
     * If true and the rotation vector sensor is available, the compass will not be used, regardless of {@link #useCompass}.
     * <p>
     * Default: false */
    public boolean useRotationVectorSensor = false;

    /** The requested sensor sampling rate in microseconds or one of the {@code SENSOR_DELAY_*} constants in {@link SensorManager}.
     * <p>
     * Default: {@link SensorManager#SENSOR_DELAY_GAME} (20 ms updates). */
    public int sensorDelay = SensorManager.SENSOR_DELAY_GAME;
}
