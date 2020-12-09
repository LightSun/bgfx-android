//
// Created by Administrator on 2020/11/28 0028.
//

#ifndef BGFX_STUDY_COLOR_H
#define BGFX_STUDY_COLOR_H

#include <string.h>
#include <stdlib.h>
#include "../lua/SkFloatBits.h"
#include "bx/bx.h"

namespace h7 {
    using Byte = unsigned char;

#define h7_COLOR_CAST(a) static_cast<Byte>(clamp(static_cast<int>(a)))

    class Color {
    public:
        union {
            struct {
                Byte _r;
                Byte _g;
                Byte _b;
                Byte _a;
            };
            Byte mem[4];
        };

        Color() : Color(0xffffffff) {
        }

        /** rgba or argb */
        Color(int val, bool _argb = false) {
            if (_argb) {
                argb8888ToColor(*this, val);
            } else {
                rgba8888ToColor(*this, val);
            }
        }

        Color(Color &in) {
            _r = in._r;
            _g = in._g;
            _b = in._b;
            _a = in._a;
        }

        Color(float r, float g, float b, float a) {
            _r = h7_COLOR_CAST(r * 255);
            _g = h7_COLOR_CAST(g * 255);
            _b = h7_COLOR_CAST(b * 255);
            _a = h7_COLOR_CAST(a * 255);
        }

        inline Color &set(Color &in) {
            _r = in._r;
            _g = in._g;
            _b = in._b;
            _a = in._a;
            return *this;
        }

        /** Sets the Color components using the specified integer value in the format RGB565. This is inverse to the rgb565(r, g, b)
	 * method.
	 *
	 * @param color The Color to be modified.
	 * @param value An integer color value in RGB565 format. */
        static inline void rgb565ToColor(Color &color, int value) {
            color._r = h7_COLOR_CAST(((value & 0x0000F800) >> 11) * 255.0f / 31.0f);
            color._g = h7_COLOR_CAST(((value & 0x000007E0) >> 5) * 255.0f / 63.0f);
            color._b = h7_COLOR_CAST(((value & 0x0000001F) >> 0) * 255.0f / 31.0f);
            //color._a = 1;
        }

        /** Sets the Color components using the specified integer value in the format RGBA4444. This is inverse to the rgba4444(r, g,
         * b, a) method.
         *
         * @param color The Color to be modified.
         * @param value An integer color value in RGBA4444 format. */
        static inline void rgba4444ToColor(Color &color, int value) {
            color._r = h7_COLOR_CAST(((value & 0x0000f000) >> 12) * 255.0f / 15.0f);
            color._g = h7_COLOR_CAST(((value & 0x00000f00) >> 8) * 255.0f / 15.0f);
            color._b = h7_COLOR_CAST(((value & 0x000000f0) >> 4) * 255.0f / 15.0f);
            color._a = h7_COLOR_CAST(((value & 0x0000000f)) * 255.0f / 15.0f);
        }

        /** Sets the Color components using the specified integer value in the format RGB888. This is inverse to the rgb888(r, g, b)
         * method.
         *
         * @param color The Color to be modified.
         * @param value An integer color value in RGB888 format. */
        static inline void rgb888ToColor(Color &color, int value) {
            color._r = h7_COLOR_CAST((value & 0x00ff0000) >> 16);
            color._g = h7_COLOR_CAST((value & 0x0000ff00) >> 8);
            color._b = h7_COLOR_CAST((value & 0x000000ff));
            //color._a = 0;
        }

        /** Sets the Color components using the specified integer value in the format RGBA8888. This is inverse to the rgba8888(r, g,
 * b, a) method.
 *
 * @param color The Color to be modified.
 * @param value An integer color value in RGBA8888 format. */
        static inline void rgba8888ToColor(Color &color, int value) {
            color._r = h7_COLOR_CAST((value & 0xff000000) >> 24);
            color._g = h7_COLOR_CAST((value & 0x00ff0000) >> 16);
            color._b = h7_COLOR_CAST((value & 0x0000ff00) >> 8);
            color._a = h7_COLOR_CAST((value & 0x000000ff));
        }

/** Sets the Color components using the specified integer value in the format ARGB8888. This is the inverse to the argb8888(a,
	 * r, g, b) method
	 *
	 * @param color The Color to be modified.
	 * @param value An integer color value in ARGB8888 format. */
        static inline void argb8888ToColor(Color &color, int value) {
            color._a = h7_COLOR_CAST((value & 0xff000000) >> 24);
            color._r = h7_COLOR_CAST((value & 0x00ff0000) >> 16);
            color._g = h7_COLOR_CAST((value & 0x0000ff00) >> 8);
            color._b = h7_COLOR_CAST((value & 0x000000ff));
        }

        /** Sets the Color components using the specified float value in the format ABGR8888.
         * @param color The Color to be modified. */
        static inline void abgr8888ToColor(Color &color, float value) {
            int c = SkFloat2Bits(value);
            color._a = h7_COLOR_CAST((c & 0xff000000) >> 24);
            color._b = h7_COLOR_CAST((c & 0x00ff0000) >> 16);
            color._g = h7_COLOR_CAST((c & 0x0000ff00) >> 8);
            color._r = h7_COLOR_CAST((c & 0x000000ff));
        }

        static inline int alpha(float alpha) {
            return (int) (alpha * 255.0f);
        }

        static inline int luminanceAlpha(float luminance, float alpha) {
            return ((int) (luminance * 255.0f) << 8) | (int) (alpha * 255);
        }

        static inline int rgb565(float r, float g, float b) {
            return ((int) (r * 31) << 11) | ((int) (g * 63) << 5) | (int) (b * 31);
        }

        static inline int rgba4444(float r, float g, float b, float a) {
            return ((int) (r * 15) << 12) | ((int) (g * 15) << 8) | ((int) (b * 15) << 4) |
                   (int) (a * 15);
        }

        static inline int rgb888(float r, float g, float b) {
            return ((int) (r * 255) << 16) | ((int) (g * 255) << 8) | (int) (b * 255);
        }

        static inline int rgba8888(float r, float g, float b, float a) {
            return ((int) (r * 255) << 24) | ((int) (g * 255) << 16) | ((int) (b * 255) << 8) |
                   (int) (a * 255);
        }

        static inline int argb8888(float a, float r, float g, float b) {
            return ((int) (a * 255) << 24) | ((int) (r * 255) << 16) | ((int) (g * 255) << 8) |
                   (int) (b * 255);
        }

        static inline int rgb565(Color &color) {
            return ((int) (color.r() * 31) << 11) | ((int) (color.g() * 63) << 5) |
                   (int) (color.b() * 31);
        }

        static inline int rgba4444(Color &color) {
            return ((int) (color.r() * 15) << 12) | ((int) (color.g() * 15) << 8) |
                   ((int) (color.b() * 15) << 4) | (int) (color.a() * 15);
        }

        static inline int rgb888(Color &color) {
            return ((int) (color._r) << 16) | ((int) (color._g) << 8) | (int) (color._b);
        }

        static inline int rgba8888(Color &color) {
            return ((int) (color._r) << 24) | ((int) (color._g * 255) << 16) |
                   ((int) (color._b * 255) << 8) | (int) (color._a * 255);
        }

        static inline int argb8888(Color &color) {
            return ((int) (color._a) << 24) | ((int) (color._r) << 16) | ((int) (color._g) << 8) |
                   (int) (color._b);
        }

        inline Color &set(const char *color) {
            char *strC;
            if (color[0] == '#') {
                strC = static_cast<char *>(malloc(strlen(color)));
                for (int i = 0; i < strlen(strC); ++i) {
                    strC[i] = color[i + 1];
                }
                strC[strlen(strC)] = '\0';

                int c = atoi(strC);
                set(c);
                free(strC);
                return *this;
            } else {
                return set(atoi(color));
            }
        }

        inline float r() {
            return _r / 255.0f;
        }

        inline float g() {
            return _g / 255.0f;
        }

        inline float b() {
            return _b / 255.0f;
        }

        inline float a() {
            return _a / 255.0f;
        }

        inline void a(float val) {
            _a = h7_COLOR_CAST(255 * val);
        }

        inline void r(float val) {
            _r = h7_COLOR_CAST(255 * val);
        }

        inline void g(float val) {
            _g = h7_COLOR_CAST(255 * val);
        }

        inline void b(float val) {
            _b = h7_COLOR_CAST(255 * val);
        }

/** Sets this Color's component values.
	 *
	 * @param r Red component
	 * @param g Green component
	 * @param b Blue component
	 * @param a Alpha component
	 *
	 * @return this Color for chaining */
        inline Color &set(float r, float g, float b, float a) {
            _r = h7_COLOR_CAST(r * 255);
            _g = h7_COLOR_CAST(g * 255);
            _b = h7_COLOR_CAST(b * 255);
            _a = h7_COLOR_CAST(a * 255);
            return *this;
        }
        inline Color &set(int r, int g, int b, int a) {
            _r = h7_COLOR_CAST(r);
            _g = h7_COLOR_CAST(g);
            _b = h7_COLOR_CAST(b);
            _a = h7_COLOR_CAST(a);
            return *this;
        }

/** Sets this color's component values through an integer representation.
	 *
	 * @return this Color for chaining
	 * @see #rgba8888ToColor(Color, int) */
        inline Color &set(int val, bool _argb = false) {
            if (_argb) {
                argb8888ToColor(*this, val);
            } else {
                rgba8888ToColor(*this, val);
            }
            return *this;
        }

        /** Adds the given color component values to this Color's values.
         *
         * @param r Red component
         * @param g Green component
         * @param b Blue component
         * @param a Alpha component
         *
         * @return this Color for chaining */

        inline Color &add(float r, float g, float b, float a) {
            _r = h7_COLOR_CAST(_r + r * 255);
            _g = h7_COLOR_CAST(_g + g * 255);
            _b = h7_COLOR_CAST(_b + b * 255);
            _a = h7_COLOR_CAST(_a + a * 255);
            return *this;
        }

/** Subtracts the given values from this Color's component values.
	 *
	 * @param r Red component
	 * @param g Green component
	 * @param b Blue component
	 * @param a Alpha component
	 *
	 * @return this Color for chaining */
        inline Color &sub(float r, float g, float b, float a) {
            _r = h7_COLOR_CAST(_r - r * 255);
            _g = h7_COLOR_CAST(_g - g * 255);
            _b = h7_COLOR_CAST(_b - b * 255);
            _a = h7_COLOR_CAST(_a - a * 255);
            return *this;
        }

        /** Multiplies this Color's color components by the given ones.
         *
         * @param r Red component
         * @param g Green component
         * @param b Blue component
         * @param a Alpha component
         *
         * @return this Color for chaining */
        inline Color &mul(float r, float g, float b, float a) {
            _r = h7_COLOR_CAST(_r * r * 255);
            _g = h7_COLOR_CAST(_g * g * 255);
            _b = h7_COLOR_CAST(_b * b * 255);
            _a = h7_COLOR_CAST(_a * a * 255);
            return *this;
        }

        /** Linearly interpolates between this color and the target color by t which is in the range [0,1]. The result is stored in
         * this color.
         * @param target The target color
         * @param t The interpolation coefficient
         * @return This color for chaining. */
        inline Color &lerp(Color &target, float t) {
            _r = h7_COLOR_CAST(_r + (int) (t * (target._r - _r)));
            _g = h7_COLOR_CAST(_g + (int) (t * (target._g - _g)));
            _b = h7_COLOR_CAST(_b + (int) (t * (target._b - _b)));
            _a = h7_COLOR_CAST(_a + (int) (t * (target._a - _a)));
            return *this;
        }

/** Linearly interpolates between this color and the target color by t which is in the range [0,1]. The result is stored in
	 * this color.
	 * @param r The red component of the target color
	 * @param g The green component of the target color
	 * @param b The blue component of the target color
	 * @param a The alpha component of the target color
	 * @param t The interpolation coefficient
	 * @return This color for chaining. */
        inline Color &lerp(float r, float g, float b, float a, float t) {
            _r = h7_COLOR_CAST(_r + (int) (t * (r * 255 - _r)));
            _g = h7_COLOR_CAST(_g + (int) (t * (g * 255 - _g)));
            _b = h7_COLOR_CAST(_b + (int) (t * (b * 255 - _b)));
            _a = h7_COLOR_CAST(_a + (int) (t * (a * 255 - _a)));
            return *this;
        }

        /** Multiplies the RGB values by the alpha. */
        inline Color &premultiplyAlpha() {
            _r *= (_a / 255.0f);
            _g *= (_a / 255.0f);
            _b *= (_b / 255.0f);
            return *this;
        }

        /** Multiplies the this color and the given color
	 *
	 * @param c the color
	 * @return this color. */
        inline Color &mul(Color &c) {
            _r = h7_COLOR_CAST(_r * c._r);
            _g = h7_COLOR_CAST(_g * c._g);
            _b = h7_COLOR_CAST(_b * c._b);
            _a = h7_COLOR_CAST(_a * c._a);
            return *this;
        }

        /** Multiplies all components of this Color with the given value.
      *
      * @param value the value
      * @return this color */
        inline Color &mul(float value) {
            _r = h7_COLOR_CAST(_r * value);
            _g = h7_COLOR_CAST(_g * value);
            _b = h7_COLOR_CAST(_b * value);
            _a = h7_COLOR_CAST(_a * value);
            return *this;
        }

        /** Adds the given color to this color.
	 *
	 * @param c the color
	 * @return this color */
        inline Color &add(Color &c) {
            _r = h7_COLOR_CAST(_r + c._r);
            _g = h7_COLOR_CAST(_g + c._g);
            _b = h7_COLOR_CAST(_b + c._b);
            _a = h7_COLOR_CAST(_a + c._a);
            return *this;
        }

        /** Subtracts the given color from this color
         *
         * @param color the color
         * @return this color */
        inline Color &sub(Color &c) {
            _r = h7_COLOR_CAST(_r - c._r);
            _g = h7_COLOR_CAST(_g - c._g);
            _b = h7_COLOR_CAST(_b - c._b);
            _a = h7_COLOR_CAST(_a - c._a);
            return *this;
        }

/** Packs the color components into a 32-bit integer with the format ARGB and then converts it to a float. Alpha is compressed
	 * from 0-255 to use only even numbers between 0-254 to avoid using float bits in the NaN range (see
	 * {@link NumberUtils#intToFloatColor(int)}). Converting a color to a float and back can be lossy for alpha.
	 * @return the packed color as a 32-bit float */
        inline float toFloatBits() {
            return SkBits2Float(toIntBits() & 0xfeffffff);
        }

        /** Packs the color components into a 32-bit integer with the format RGBA.
	 * @return the packed color as a 32-bit int. */
        inline int toIntBits(bool argb = false) {
            if (argb) {
                return ((int) (_a) << 24) | ((int) (_r) << 16) | ((int) (_g) << 8) | ((int) (_b));
            } else {
                return ((int) (_r) << 24) | ((int) (_g) << 16) | ((int) (_b) << 8) | ((int) (_a));
            }
        }

        /** Sets the RGB Color components using the specified Hue-Saturation-Value. Note that HSV components are voluntary not clamped
	 * to preserve high range color and can range beyond typical values.
	 * @param h The Hue in degree from 0 to 360
	 * @param s The Saturation from 0 to 1
	 * @param v The Value (brightness) from 0 to 1
	 * @return The modified Color for chaining. */
        inline Color &fromHsv(float h, float s, float v) {
            //float x = (h / 60.0f + 6) % 6.0f;
            float x = skf_mod((h / 60.0f + 6), 6.0f);
            int i = (int) x;
            float f = x - i;
            float p = v * (1 - s);
            float q = v * (1 - s * f);
            float t = v * (1 - s * (1 - f));

            Byte r;
            Byte g;
            Byte b;
            switch (i) {
                case 0:
                    r = h7_COLOR_CAST(v * 255);
                    g = h7_COLOR_CAST(t * 255);
                    b = h7_COLOR_CAST(p * 255);
                    break;
                case 1:
                    r = h7_COLOR_CAST(q * 255);
                    g = h7_COLOR_CAST(v * 255);
                    b = h7_COLOR_CAST(p * 255);
                    break;
                case 2:
                    r = h7_COLOR_CAST(p * 255);
                    g = h7_COLOR_CAST(v * 255);
                    b = h7_COLOR_CAST(t * 255);
                    break;
                case 3:
                    r = h7_COLOR_CAST(p * 255);
                    g = h7_COLOR_CAST(q * 255);
                    b = h7_COLOR_CAST(v * 255);
                    break;
                case 4:
                    r = h7_COLOR_CAST(t * 255);
                    g = h7_COLOR_CAST(p * 255);
                    b = h7_COLOR_CAST(v * 255);
                    break;
                default:
                    r = h7_COLOR_CAST(v * 255);
                    g = h7_COLOR_CAST(p * 255);
                    b = h7_COLOR_CAST(q * 255);
            }
            _r = r;
            _g = g;
            _b = b;
            return *this;
        }

        /** Sets RGB components using the specified Hue-Saturation-Value. This is a convenient method for
         * {@link #fromHsv(float, float, float)}. This is the inverse of {@link #toHsv(float[])}.
         * @param hsv The Hue, Saturation and Value components in that order.
         * @return The modified Color for chaining. */
        inline Color &fromHsv(float hsv[3]) {
            return fromHsv(hsv[0], hsv[1], hsv[2]);
        }

        /** Extract Hue-Saturation-Value. This is the inverse of {@link #fromHsv(float[])}.
         * @param hsv The HSV array to be modified.
         * @return HSV components for chaining. */
        inline void toHsv(float hsv[3]) {
            float max = bx::max(_r, _g, _b) / 255.0f;
            float min = bx::min(_r, _g, _b) / 255.0f;
            float range = max - min;
            if (range == 0) {
                hsv[0] = 0;
            } else if (max == r()) {
                // hsv[0] = (60 * (g() - b()) / range + 360) % 360;
                hsv[0] = skf_mod((60 * (g() - b()) / range + 360), 360.0f);
            } else if (max == g()) {
                hsv[0] = 60 * (b() - r()) / range + 120;
            } else {
                hsv[0] = 60 * (r() - g()) / range + 240;
            }
            if (max > 0) {
                hsv[1] = 1 - min / max;
            } else {
                hsv[1] = 0;
            }
            hsv[2] = max;
        }
        static inline int clamp(int com) {
            if (com < 0) {
                com = 0;
            }
            if (com > 255) {
                com = 255;
            }
            return com;
        }

        //===================== operators ============
        inline Byte &operator[](int index) { return mem[index]; }

        inline const Byte operator[](int index) const { return mem[index]; }

        inline bool operator==(const Color &color) {
            return toIntBits() == const_cast<Color &>(color).toIntBits();
        }
        //inline bool operator<(const Color& color){ return toIntBits() < color.toIntBits(); }

        Color &operator=(const unsigned int color) { return set(color); }

        Color &operator+=(const Color &c) {
            return add(const_cast<Color &>(c));
        }

        Color &operator-=(const Color &color) {
            return sub(const_cast<Color &>(color));
        }

        Color &operator*=(const Color &color) {
            return mul(const_cast<Color &>(color));
        }

        Color operator+(const Color &color) {
            Color ret(*this);
            ret += color;
            return ret;
        }

        Color operator-(const Color &color) {
            Color ret(*this);
            ret -= color;
            return ret;
        }

        Color operator*(const Color &color) {
            Color ret(*this);
            ret *= color;
            return ret;
        }

    };

    //rgba 8888
    static constexpr int COLOR_WHITE = 0xffffffff;
    static constexpr int COLOR_LIGHT_GRAY = 0xbfbfbfff;
    static constexpr int COLOR_GRAY = 0x7f7f7fff;
    static constexpr int COLOR_DARK_GRAY = 0x3f3f3fff;
    static constexpr int COLOR_BLACK = 0x000000ff;

    static constexpr int COLOR_CLEAR = 0x00000000;
    static constexpr int COLOR_BLUE = 0x0000ffff;
    static constexpr int COLOR_NAVY = 0x00007fff;
}
#endif //BGFX_STUDY_COLOR_H
