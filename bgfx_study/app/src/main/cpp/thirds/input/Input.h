//
// Created by Administrator on 2020/11/23 0023.
//

#ifndef BGFX_STUDY_INPUT_H
#define BGFX_STUDY_INPUT_H

//#include "map2.h"
#include "../../include/map2.h"
#include "InputProcessor.h"
#include "RefObject.h"
#include "utils/Array.h"

namespace h7 {

    class KeyEvent : public RefObject {
    public:
        static const int KEY_DOWN = 0;
        static const int KEY_UP = 1;
        static const int KEY_TYPED = 2;
        //relative android
        static const int KEYCODE_BACK = 4;
        static const int KEYCODE_UNKNOWN = 0;
        static const int ACTION_MULTIPLE = 2;
        static const int ACTION_DOWN = 0;
        static const int ACTION_UP = 1;

        long long timeStamp;
        int type;
        int keyCode;
        char keyChar;

        class Iterator: public ArrayIterator<KeyEvent*>{
        public:
            bool iterate(Array<KeyEvent*>* arr, int index, KeyEvent*& ele){
                ele->unRefAndDestroy();
                return false;
            }
        };
    };

    class TouchEvent :public RefObject {
    public:
        static const int TOUCH_DOWN = 0;
        static const int TOUCH_UP = 1;
        static const int TOUCH_DRAGGED = 2;
        static const int TOUCH_SCROLLED = 3;
        static const int TOUCH_MOVED = 4;

        long long timeStamp;
        int type;
        int x;
        int y;
        int scrollAmountX;
        int scrollAmountY;
        int button;
        int pointer;

        class Iterator: public ArrayIterator<TouchEvent*>{
        public:
            bool iterate(Array<TouchEvent*>* arr, int index, TouchEvent*& ele){
                ele->unRefAndDestroy();
                return false;
            }
        };
    };

    enum Buttons {
        LEFT = 0,
        RIGHT = 1,
        MIDDLE = 2,
        BACK = 3,
        FORWARD = 4
    };

    enum Peripheral {
        HardwareKeyboard,
        OnscreenKeyboard,
        MultitouchScreen,
        Accelerometer,
        Compass,
        Vibrator,
        Gyroscope,
        RotationVector,
        Pressure
    };
    enum OnscreenKeyboardType {
        Default = 0, NumberPad = 1, PhonePad = 2, Email = 3, Password = 4, URI = 5
    };
    enum Orientation {
        Landscape = 0, Portrait = 1, Count = 2
    };

/** <p>
* Interface to the input facilities. This allows polling the state of the keyboard, the touch screen and the accelerometer. On
* some backends (desktop, gwt, etc) the touch screen is replaced by mouse input. The accelerometer is of course not available on
* all backends.
* </p>
*
* <p>
* Instead of polling for events, one can process all input events with an {@link InputProcessor}. You can set the InputProcessor
* via the {@link #setInputProcessor(InputProcessor)} method. It will be called before the {@link ApplicationListener#render()}
* method in each frame.
* </p>
*
* <p>
* Keyboard keys are translated to the constants in {@link Keys} transparently on all systems. Do not use system specific key
* constants.
* </p>
*
* <p>
* The class also offers methods to use (and test for the presence of) other input systems like vibration, compass, on-screen
* keyboards, and cursor capture. Support for simple input dialogs is also provided.
* </p>
 * @author mzechner
* */
    class Input {
    public:
        class Keys {
        public:
            constexpr static int ANY_KEY = -1;
            constexpr static int NUM_0 = 7;
            constexpr static int NUM_1 = 8;
            constexpr static int NUM_2 = 9;
            constexpr static int NUM_3 = 10;
            constexpr static int NUM_4 = 11;
            constexpr static int NUM_5 = 12;
            constexpr static int NUM_6 = 13;
            constexpr static int NUM_7 = 14;
            constexpr static int NUM_8 = 15;
            constexpr static int NUM_9 = 16;
            constexpr static int A = 29;
            constexpr static int ALT_LEFT = 57;
            constexpr static int ALT_RIGHT = 58;
            constexpr static int APOSTROPHE = 75;
            constexpr static int AT = 77;
            constexpr static int B = 30;
            constexpr static int BACK = 4;
            constexpr static int BACKSLASH = 73;
            constexpr static int C = 31;
            constexpr static int CALL = 5;
            constexpr static int CAMERA = 27;
            constexpr static int CLEAR = 28;
            constexpr static int COMMA = 55;
            constexpr static int D = 32;
            constexpr static int DEL = 67;
            constexpr static int BACKSPACE = 67;
            constexpr static int FORWARD_DEL = 112;
            constexpr static int DPAD_CENTER = 23;
            constexpr static int DPAD_DOWN = 20;
            constexpr static int DPAD_LEFT = 21;
            constexpr static int DPAD_RIGHT = 22;
            constexpr static int DPAD_UP = 19;
            constexpr static int CENTER = 23;
            constexpr static int DOWN = 20;
            constexpr static int LEFT = 21;
            constexpr static int RIGHT = 22;
            constexpr static int UP = 19;
            constexpr static int E = 33;
            constexpr static int ENDCALL = 6;
            constexpr static int ENTER = 66;
            constexpr static int ENVELOPE = 65;
            constexpr static int EQUALS = 70;
            constexpr static int EXPLORER = 64;
            constexpr static int F = 34;
            constexpr static int FOCUS = 80;
            constexpr static int G = 35;
            constexpr static int GRAVE = 68;
            constexpr static int H = 36;
            constexpr static int HEADSETHOOK = 79;
            constexpr static int HOME = 3;
            constexpr static int I = 37;
            constexpr static int J = 38;
            constexpr static int K = 39;
            constexpr static int L = 40;
            constexpr static int LEFT_BRACKET = 71;
            constexpr static int M = 41;
            constexpr static int MEDIA_FAST_FORWARD = 90;
            constexpr static int MEDIA_NEXT = 87;
            constexpr static int MEDIA_PLAY_PAUSE = 85;
            constexpr static int MEDIA_PREVIOUS = 88;
            constexpr static int MEDIA_REWIND = 89;
            constexpr static int MEDIA_STOP = 86;
            constexpr static int MENU = 82;
            constexpr static int MINUS = 69;
            constexpr static int MUTE = 91;
            constexpr static int N = 42;
            constexpr static int NOTIFICATION = 83;
            constexpr static int NUM = 78;
            constexpr static int O = 43;
            constexpr static int P = 44;
            constexpr static int PERIOD = 56;
            constexpr static int PLUS = 81;
            constexpr static int POUND = 18;
            constexpr static int POWER = 26;
            constexpr static int Q = 45;
            constexpr static int R = 46;
            constexpr static int RIGHT_BRACKET = 72;
            constexpr static int S = 47;
            constexpr static int SEARCH = 84;
            constexpr static int SEMICOLON = 74;
            constexpr static int SHIFT_LEFT = 59;
            constexpr static int SHIFT_RIGHT = 60;
            constexpr static int SLASH = 76;
            constexpr static int SOFT_LEFT = 1;
            constexpr static int SOFT_RIGHT = 2;
            constexpr static int SPACE = 62;
            constexpr static int STAR = 17;
            constexpr static int SYM = 63;
            constexpr static int T = 48;
            constexpr static int TAB = 61;
            constexpr static int U = 49;
            constexpr static int UNKNOWN = 0;
            constexpr static int V = 50;
            constexpr static int VOLUME_DOWN = 25;
            constexpr static int VOLUME_UP = 24;
            constexpr static int W = 51;
            constexpr static int X = 52;
            constexpr static int Y = 53;
            constexpr static int Z = 54;
            constexpr static int META_ALT_LEFT_ON = 16;
            constexpr static int META_ALT_ON = 2;
            constexpr static int META_ALT_RIGHT_ON = 32;
            constexpr static int META_SHIFT_LEFT_ON = 64;
            constexpr static int META_SHIFT_ON = 1;
            constexpr static int META_SHIFT_RIGHT_ON = 128;
            constexpr static int META_SYM_ON = 4;
            constexpr static int CONTROL_LEFT = 129;
            constexpr static int CONTROL_RIGHT = 130;
            constexpr static int ESCAPE = 131;
            constexpr static int END = 132;
            constexpr static int INSERT = 133;
            constexpr static int PAGE_UP = 92;
            constexpr static int PAGE_DOWN = 93;
            constexpr static int PICTSYMBOLS = 94;
            constexpr static int SWITCH_CHARSET = 95;
            constexpr static int BUTTON_CIRCLE = 255;
            constexpr static int BUTTON_A = 96;
            constexpr static int BUTTON_B = 97;
            constexpr static int BUTTON_C = 98;
            constexpr static int BUTTON_X = 99;
            constexpr static int BUTTON_Y = 100;
            constexpr static int BUTTON_Z = 101;
            constexpr static int BUTTON_L1 = 102;
            constexpr static int BUTTON_R1 = 103;
            constexpr static int BUTTON_L2 = 104;
            constexpr static int BUTTON_R2 = 105;
            constexpr static int BUTTON_THUMBL = 106;
            constexpr static int BUTTON_THUMBR = 107;
            constexpr static int BUTTON_START = 108;
            constexpr static int BUTTON_SELECT = 109;
            constexpr static int BUTTON_MODE = 110;

            constexpr static int NUMPAD_0 = 144;
            constexpr static int NUMPAD_1 = 145;
            constexpr static int NUMPAD_2 = 146;
            constexpr static int NUMPAD_3 = 147;
            constexpr static int NUMPAD_4 = 148;
            constexpr static int NUMPAD_5 = 149;
            constexpr static int NUMPAD_6 = 150;
            constexpr static int NUMPAD_7 = 151;
            constexpr static int NUMPAD_8 = 152;
            constexpr static int NUMPAD_9 = 153;

// const static int BACKTICK = 0;
// const static int TILDE = 0;
// const static int UNDERSCORE = 0;
// const static int DOT = 0;
// const static int BREAK = 0;
// const static int PIPE = 0;
// const static int EXCLAMATION = 0;
// const static int QUESTIONMARK = 0;

// ` | VK_BACKTICK
// ~ | VK_TILDE
// : | VK_COLON
// _ | VK_UNDERSCORE
// . | VK_DOT
// (break) | VK_BREAK
// | | VK_PIPE
// ! | VK_EXCLAMATION
// ? | VK_QUESTION
            constexpr static int COLON = 243;
            constexpr static int F1 = 244;
            constexpr static int F2 = 245;
            constexpr static int F3 = 246;
            constexpr static int F4 = 247;
            constexpr static int F5 = 248;
            constexpr static int F6 = 249;
            constexpr static int F7 = 250;
            constexpr static int F8 = 251;
            constexpr static int F9 = 252;
            constexpr static int F10 = 253;
            constexpr static int F11 = 254;
            constexpr static int F12 = 255;

            constexpr static int MAX_KEYCODE = 255;

            /** @return a human readable representation of the keycode. The returned value can be used in
             *         {@link Input.Keys#valueOf(const char *)} */
            inline static const char *toString(int keycode) {
                //if (keycode < 0) throw new IllegalArgumentException("keycode cannot be negative, keycode: " + keycode);
                //if (keycode > MAX_KEYCODE) throw new IllegalArgumentException("keycode cannot be greater than 255, keycode: " + keycode);
                if (keycode < 0 || keycode > MAX_KEYCODE) return nullptr;
                switch (keycode) {
                    // META* variables should not be used with this method.
                    case UNKNOWN:
                        return "Unknown";
                    case SOFT_LEFT:
                        return "Soft Left";
                    case SOFT_RIGHT:
                        return "Soft Right";
                    case HOME:
                        return "Home";
                    case BACK:
                        return "Back";
                    case CALL:
                        return "Call";
                    case ENDCALL:
                        return "End Call";
                    case NUM_0:
                        return "0";
                    case NUM_1:
                        return "1";
                    case NUM_2:
                        return "2";
                    case NUM_3:
                        return "3";
                    case NUM_4:
                        return "4";
                    case NUM_5:
                        return "5";
                    case NUM_6:
                        return "6";
                    case NUM_7:
                        return "7";
                    case NUM_8:
                        return "8";
                    case NUM_9:
                        return "9";
                    case STAR:
                        return "*";
                    case POUND:
                        return "#";
                    case UP:
                        return "Up";
                    case DOWN:
                        return "Down";
                    case LEFT:
                        return "Left";
                    case RIGHT:
                        return "Right";
                    case CENTER:
                        return "Center";
                    case VOLUME_UP:
                        return "Volume Up";
                    case VOLUME_DOWN:
                        return "Volume Down";
                    case POWER:
                        return "Power";
                    case CAMERA:
                        return "Camera";
                    case CLEAR:
                        return "Clear";
                    case A:
                        return "A";
                    case B:
                        return "B";
                    case C:
                        return "C";
                    case D:
                        return "D";
                    case E:
                        return "E";
                    case F:
                        return "F";
                    case G:
                        return "G";
                    case H:
                        return "H";
                    case I:
                        return "I";
                    case J:
                        return "J";
                    case K:
                        return "K";
                    case L:
                        return "L";
                    case M:
                        return "M";
                    case N:
                        return "N";
                    case O:
                        return "O";
                    case P:
                        return "P";
                    case Q:
                        return "Q";
                    case R:
                        return "R";
                    case S:
                        return "S";
                    case T:
                        return "T";
                    case U:
                        return "U";
                    case V:
                        return "V";
                    case W:
                        return "W";
                    case X:
                        return "X";
                    case Y:
                        return "Y";
                    case Z:
                        return "Z";
                    case COMMA:
                        return ",";
                    case PERIOD:
                        return ".";
                    case ALT_LEFT:
                        return "L-Alt";
                    case ALT_RIGHT:
                        return "R-Alt";
                    case SHIFT_LEFT:
                        return "L-Shift";
                    case SHIFT_RIGHT:
                        return "R-Shift";
                    case TAB:
                        return "Tab";
                    case SPACE:
                        return "Space";
                    case SYM:
                        return "SYM";
                    case EXPLORER:
                        return "Explorer";
                    case ENVELOPE:
                        return "Envelope";
                    case ENTER:
                        return "Enter";
                    case DEL:
                        return "Delete"; // also BACKSPACE
                    case GRAVE:
                        return "`";
                    case MINUS:
                        return "-";
                    case EQUALS:
                        return "=";
                    case LEFT_BRACKET:
                        return "[";
                    case RIGHT_BRACKET:
                        return "]";
                    case BACKSLASH:
                        return "\\";
                    case SEMICOLON:
                        return ";";
                    case APOSTROPHE:
                        return "'";
                    case SLASH:
                        return "/";
                    case AT:
                        return "@";
                    case NUM:
                        return "Num";
                    case HEADSETHOOK:
                        return "Headset Hook";
                    case FOCUS:
                        return "Focus";
                    case PLUS:
                        return "Plus";
                    case MENU:
                        return "Menu";
                    case NOTIFICATION:
                        return "Notification";
                    case SEARCH:
                        return "Search";
                    case MEDIA_PLAY_PAUSE:
                        return "Play/Pause";
                    case MEDIA_STOP:
                        return "Stop Media";
                    case MEDIA_NEXT:
                        return "Next Media";
                    case MEDIA_PREVIOUS:
                        return "Prev Media";
                    case MEDIA_REWIND:
                        return "Rewind";
                    case MEDIA_FAST_FORWARD:
                        return "Fast Forward";
                    case MUTE:
                        return "Mute";
                    case PAGE_UP:
                        return "Page Up";
                    case PAGE_DOWN:
                        return "Page Down";
                    case PICTSYMBOLS:
                        return "PICTSYMBOLS";
                    case SWITCH_CHARSET:
                        return "SWITCH_CHARSET";
                    case BUTTON_A:
                        return "A Button";
                    case BUTTON_B:
                        return "B Button";
                    case BUTTON_C:
                        return "C Button";
                    case BUTTON_X:
                        return "X Button";
                    case BUTTON_Y:
                        return "Y Button";
                    case BUTTON_Z:
                        return "Z Button";
                    case BUTTON_L1:
                        return "L1 Button";
                    case BUTTON_R1:
                        return "R1 Button";
                    case BUTTON_L2:
                        return "L2 Button";
                    case BUTTON_R2:
                        return "R2 Button";
                    case BUTTON_THUMBL:
                        return "Left Thumb";
                    case BUTTON_THUMBR:
                        return "Right Thumb";
                    case BUTTON_START:
                        return "Start";
                    case BUTTON_SELECT:
                        return "Select";
                    case BUTTON_MODE:
                        return "Button Mode";
                    case FORWARD_DEL:
                        return "Forward Delete";
                    case CONTROL_LEFT:
                        return "L-Ctrl";
                    case CONTROL_RIGHT:
                        return "R-Ctrl";
                    case ESCAPE:
                        return "Escape";
                    case END:
                        return "End";
                    case INSERT:
                        return "Insert";
                    case NUMPAD_0:
                        return "Numpad 0";
                    case NUMPAD_1:
                        return "Numpad 1";
                    case NUMPAD_2:
                        return "Numpad 2";
                    case NUMPAD_3:
                        return "Numpad 3";
                    case NUMPAD_4:
                        return "Numpad 4";
                    case NUMPAD_5:
                        return "Numpad 5";
                    case NUMPAD_6:
                        return "Numpad 6";
                    case NUMPAD_7:
                        return "Numpad 7";
                    case NUMPAD_8:
                        return "Numpad 8";
                    case NUMPAD_9:
                        return "Numpad 9";
                    case COLON:
                        return ":";
                    case F1:
                        return "F1";
                    case F2:
                        return "F2";
                    case F3:
                        return "F3";
                    case F4:
                        return "F4";
                    case F5:
                        return "F5";
                    case F6:
                        return "F6";
                    case F7:
                        return "F7";
                    case F8:
                        return "F8";
                    case F9:
                        return "F9";
                    case F10:
                        return "F10";
                    case F11:
                        return "F11";
                    case F12:
                        return "F12";
                        // BUTTON_CIRCLE unhandled, as it conflicts with the more likely to be pressed F12
                    default:
                        // key name not found
                        return nullptr;
                }
            }

            /** @param keyname the keyname returned by the {@link Keys#toconst char *(int)} method
             * @return the int keycode */
            inline static int valueOf(const char *keyname) {
                initializeKeyNames();
                return keyNames.get(keyname);
            }

            /** lazily intialized in {@link Keys#valueOf(const char *)} */
        private:
            static h7::Map2<const char *, int> keyNames;

            inline static void initializeKeyNames() {
                for (int i = 0; i < 256; i++) {
                    const char *name = toString(i);
                    if (name != nullptr) keyNames.put(name, i);
                }
            }
        };

        /** @return The acceleration force in m/s^2 applied to the device in the X axis, including the force of gravity */
        virtual float getAccelerometerX() = 0;

        /** @return The acceleration force in m/s^2 applied to the device in the Y axis, including the force of gravity */
        virtual float getAccelerometerY() = 0;

        /** @return The acceleration force in m/s^2 applied to the device in the Z axis, including the force of gravity */
        virtual float getAccelerometerZ() = 0;

        /** @return The rate of rotation in rad/s around the X axis */
        virtual float getGyroscopeX() = 0;

        /** @return The rate of rotation in rad/s around the Y axis */
        virtual float getGyroscopeY() = 0;

        /** @return The rate of rotation in rad/s around the Z axis */
        virtual float getGyroscopeZ() = 0;

        /** @return The maximum number of pointers supported */
        virtual int getMaxPointers() = 0;

        /** @return The x coordinate of the last touch on touch screen devices and the current mouse position on desktop for the first
         *         pointer in screen coordinates. The screen origin is the top left corner. */
        virtual int getX() = 0;

        /** Returns the x coordinate in screen coordinates of the given pointer. Pointers are indexed from 0 to n. The pointer id
         * identifies the order in which the fingers went down on the screen, e.g. 0 is the first finger, 1 is the second and so on.
         * When two fingers are touched down and the first one is lifted the second one keeps its index. If another finger is placed on
         * the touch screen the first free index will be used.
         *
         * @param pointer the pointer id.
         * @return the x coordinate */
        virtual int getX(int pointer) = 0;

        /** @return the different between the current pointer location and the last pointer location on the x-axis. */
        virtual int getDeltaX() = 0;

        /** @return the different between the current pointer location and the last pointer location on the x-axis. */
        virtual int getDeltaX(int pointer) = 0;

        /** @return The y coordinate of the last touch on touch screen devices and the current mouse position on desktop for the first
         *         pointer in screen coordinates. The screen origin is the top left corner. */
        virtual int getY() = 0;

        /** Returns the y coordinate in screen coordinates of the given pointer. Pointers are indexed from 0 to n. The pointer id
         * identifies the order in which the fingers went down on the screen, e.g. 0 is the first finger, 1 is the second and so on.
         * When two fingers are touched down and the first one is lifted the second one keeps its index. If another finger is placed on
         * the touch screen the first free index will be used.
         *
         * @param pointer the pointer id.
         * @return the y coordinate */
        virtual int getY(int pointer) = 0;

        /** @return the different between the current pointer location and the last pointer location on the y-axis. */
        virtual int getDeltaY() = 0;

        /** @return the different between the current pointer location and the last pointer location on the y-axis. */
        virtual int getDeltaY(int pointer) = 0;

        /** @return whether the screen is currently touched. */
        virtual bool isTouched() = 0;

        /** @return whether a new touch down event just occurred. */
        virtual bool justTouched() = 0;

        /** Whether the screen is currently touched by the pointer with the given index. Pointers are indexed from 0 to n. The pointer
         * id identifies the order in which the fingers went down on the screen, e.g. 0 is the first finger, 1 is the second and so on.
         * When two fingers are touched down and the first one is lifted the second one keeps its index. If another finger is placed on
         * the touch screen the first free index will be used.
         *
         * @param pointer the pointer
         * @return whether the screen is touched by the pointer */
        virtual bool isTouched(int pointer) = 0;

        /** @return the pressure of the first pointer */
        virtual float getPressure() = 0;

        /** Returns the pressure of the given pointer, where 0 is untouched. On Android it should be
         * up to 1.0, but it can go above that slightly and its not consistent between devices. On iOS 1.0 is the normal touch
         * and significantly more of hard touch. Check relevant manufacturer documentation for details.
         * Check availability with {@link Input#isPeripheralAvailable(Peripheral)}. If not supported, returns 1.0 when touched.
         *
         * @param pointer the pointer id.
         * @return the pressure */
        virtual float getPressure(int pointer) = 0;

        /** Whether a given button is pressed or not. Button constants can be found in {@link Buttons}. On Android only the Buttons#LEFT
         * constant is meaningful before version 4.0.
         * @param button the button to check.
         * @return whether the button is down or not. */
        virtual bool isButtonPressed(int button) = 0;

        /** Returns whether a given button has just been pressed. Button constants can be found in {@link Buttons}. On Android only the Buttons#LEFT
         * constant is meaningful before version 4.0. On WebGL (GWT), only LEFT, RIGHT and MIDDLE buttons are supported.
         *
         * @param button the button to check.
         * @return true or false. */
        virtual bool isButtonJustPressed(int button) = 0;

        /** Returns whether the key is pressed.
         *
         * @param key The key code as found in {@link Input.Keys}.
         * @return true or false. */
        virtual bool isKeyPressed(int key) = 0;

        /** Returns whether the key has just been pressed.
         *
         * @param key The key code as found in {@link Input.Keys}.
         * @return true or false. */
        virtual bool isKeyJustPressed(int key) = 0;

        /** Sets the on-screen keyboard visible if available. Will use the Default keyboard type.
         *
         * @param visible visible or not */
        virtual void setOnscreenKeyboardVisible(bool visible) = 0;

        /** Sets the on-screen keyboard visible if available.
         *
         * @param visible visible or not
         * @param type which type of keyboard we wish to display. Can be null when hiding */
        virtual void setOnscreenKeyboardVisible(bool visible, OnscreenKeyboardType type) = 0;

        /** Vibrates for the given amount of time. Note that you'll need the permission
         * <code> <uses-permission android:name="android.permission.VIBRATE" /></code> in your manifest file in order for this to work.
         *
         * @param milliseconds the number of milliseconds to vibrate. */
        virtual void vibrate(int milliseconds) = 0;

        /** Vibrate with a given pattern. Pass in an array of ints that are the times at which to turn on or off the vibrator. The first
         * one is how long to wait before turning it on, and then after that it alternates. If you want to repeat, pass the index into
         * the pattern at which to start the repeat.
         * @param pattern an array of longs of times to turn the vibrator on or off.
         * @param repeat the index into pattern at which to repeat, or -1 if you don't want to repeat. */
        virtual void vibrate(long long *pattern, int patLen, int repeat) = 0;

        /** Stops the vibrator */
        virtual void cancelVibrate() = 0;

        /** The azimuth is the angle of the device's orientation around the z-axis. The positive z-axis points towards the earths
         * center.
         *
         * @see <a
         *      href="http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])">http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])</a>
         * @return the azimuth in degrees */
        virtual float getAzimuth() = 0;

        /** The pitch is the angle of the device's orientation around the x-axis. The positive x-axis roughly points to the west and is
         * orthogonal to the z- and y-axis.
         * @see <a
         *      href="http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])">http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])</a>
         * @return the pitch in degrees */
        virtual float getPitch() = 0;

        /** The roll is the angle of the device's orientation around the y-axis. The positive y-axis points to the magnetic north pole
         * of the earth.
         * @see <a
         *      href="http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])">http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])</a>
         * @return the roll in degrees */
        virtual float getRoll() = 0;

        /** Returns the rotation matrix describing the devices rotation as per <a href=
         * "http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])"
         * >SensorManager#getRotationMatrix(float[], float[], float[], float[])</a>. Does not manipulate the matrix if the platform
         * does not have an accelerometer.
         * the len of matrix must be 9.
         * @param matrix */
        virtual void getRotationMatrix(float *matrix) = 0;

        /** @return the time of the event currently reported to the {@link InputProcessor}. */
        virtual long long getCurrentEventTime() = 0;
        /**
         * //@Deprecated use {@link Input#setCatchKey(int keycode, bool catchKey)} instead
         *
         * Sets whether the BACK button on Android should be caught. This will prevent the app from being paused. Will have no effect
         * on the desktop.
         *
         * @param catchBack whether to catch the back button */
        //@Deprecated
        virtual void setCatchBackKey(bool catchBack) = 0;

        /**
         * //@Deprecated use {@link Input#isCatchKey(int keycode)} instead
         * @return whether the back button is currently being caught */
        //@Deprecated
        virtual bool isCatchBackKey() = 0;

        /**
         * //@Deprecated use {@link Input#setCatchKey(int keycode, bool catchKey)} instead
         *
         * Sets whether the MENU button on Android should be caught. This will prevent the onscreen keyboard to show up. Will have no
         * effect on the desktop.
         *
         * @param catchMenu whether to catch the menu button */
        //@Deprecated
        virtual void setCatchMenuKey(bool catchMenu) = 0;

        /**
         * //@Deprecated use {@link Input#isCatchKey(int keycode)} instead
         * @return whether the menu button is currently being caught */
        //@Deprecated
        virtual bool isCatchMenuKey() = 0;

        /**
         * Sets whether the given key on Android should be caught. No effect on other platforms.
         * All keys that are not caught may be handled by other apps or background processes. For example, media or volume
         * buttons are handled by background media players if present. If you use these keys to control your game, they
         * must be catched to prevent unintended behaviour.
         *
         * @param keycode  keycode to catch
         * @param catchKey whether to catch the given keycode
         */
        virtual void setCatchKey(int keycode, bool catchKey) = 0;

        /**
         *
         * @param keycode keycode to check if caught
         * @return true if the given keycode is configured to be caught
         */
        virtual bool isCatchKey(int keycode) = 0;

        /** Sets the {@link InputProcessor} that will receive all touch and key input events. It will be called before the
         * {@link ApplicationListener#render()} method each frame.
         *
         * @param processor the InputProcessor */
        virtual void setInputProcessor(InputProcessor *processor) = 0;

        /** @return the currently set {@link InputProcessor} or null. */
        virtual InputProcessor *getInputProcessor() = 0;

        /** Queries whether a {@link Peripheral} is currently available. In case of Android and the {@link Peripheral#HardwareKeyboard}
         * this returns the whether the keyboard is currently slid out or not.
         *
         * @param peripheral the {@link Peripheral}
         * @return whether the peripheral is available or not. */
        virtual bool isPeripheralAvailable(Peripheral peripheral) = 0;

        /** @return the rotation of the device with respect to its native orientation. */
        virtual int getRotation() = 0;

        /** @return the native orientation of the device. */
        virtual Orientation getNativeOrientation() = 0;

        /** Only viable on the desktop. Will confine the mouse cursor location to the window and hide the mouse cursor. X and y
         * coordinates are still reported as if the mouse was not catched.
         * @param catched whether to catch or not to catch the mouse cursor */
        virtual void setCursorCatched(bool catched) = 0;

        /** @return whether the mouse cursor is catched. */
        virtual bool isCursorCatched() = 0;

        /** Only viable on the desktop. Will set the mouse cursor location to the given window coordinates (origin top-left corner).
         * @param x the x-position
         * @param y the y-position */
        virtual void setCursorPosition(int x, int y) = 0;
    };

}

#endif //BGFX_STUDY_INPUT_H
