# bgfx-android

# Build bgfx-android on Windows
* 1, clone libs:
```
git clone git://github.com/bkaradzic/bx.git
git clone git://github.com/bkaradzic/bimg.git
git clone git://github.com/bkaradzic/bgfx.git
```
* 2, create 'build_android.bat' in bgfx dir
contents are:
```
SET ANDROID_NDK_ROOT=F:/study/android/android-ndk-r19c
SET ANDROID_NDK_CLANG=%ANDROID_NDK_ROOT%/toolchains/llvm/prebuilt/windows-x86_64
SET ANDROID_NDK_ARM=%ANDROID_NDK_ROOT%/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64
SET ANDROID_NDK_ARM64=%ANDROID_NDK_ROOT%/toolchains/aarch64-linux-android-4.9/prebuilt/windows-x86_64
SET ANDROID_NDK_MIPS=%ANDROID_NDK_ROOT%/toolchains/mipsel-linux-android-4.9/prebuilt/windows-x86_64
SET ANDROID_NDK_X86=%ANDROID_NDK_ROOT%/toolchains/x86-4.9/prebuilt/windows-x86_64
SET ANDROID_SDK_ROOT=F:/study/android_sdk
SET GIT_BIN=F:/Program Files/Git/usr/bin
SET PATH=%PATH%;%ANDROID_SDK_ROOT%/platform-tools;%GIT_BIN%
make
make android-arm & make android-arm64 & make android-x86
@PAUSE
```
