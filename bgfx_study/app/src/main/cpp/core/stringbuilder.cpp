/*
 * Copyright 2017 Ryan Armstrong <ryan@cavaliercoder.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * C String Builder - https://github.com/cavaliercoder/c-stringbuilder
 *
 * sb.c is a simple, non-thread safe String Builder that makes use of a
 * dynamically-allocated linked-list to enable linear time appending and
 * concatenation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "stringbuilder.h"
#include "stringbuilder_pri.h"

namespace SB {
    static const std::map<unsigned char,const char*> _DEF_FORMATS = {
            {T_BOOL, "%d"},
            {T_CHAR, "%c"},
            {T_U_CHAR, "%u"},
            {T_U_INT, "%u"},
            {T_INT, "%d"},
            {T_LONG, "%ld"},
            {T_LONG_LONG, "%lld"},
            {T_U_LONG, "%lu"},
            {T_U_LONG_LONG, "%llu"},
            {T_FLOAT, "%f"},
            {T_DOUBLE, "%lf"},
            {T_SHORT, "%d"},
            {T_U_SHORT, "%u"},
    };
#define _D_MAP (static_cast<std::map<unsigned char,const char*>>(_DEF_FORMATS))
/*
 * sb_create returns a pointer to a new StringBuilder or NULL if memory is not
 * available.
 */
    StringBuilder::StringBuilder() : StringBuilder(_D_MAP) {
    }
    StringBuilder::StringBuilder(std::map<unsigned char, const char *> formatMap) : root(nullptr), trunk(nullptr), m_length(0) {
        m_format = formatMap;
    }

    StringBuilder::~StringBuilder() {
        reset();
    }

/*
 * sb_reset resets the given StringBuilder, freeing all previously appended
 * strings.
 */
    void StringBuilder::reset() {
        StringFragment *frag = NULL;
        StringFragment *next = NULL;

        frag = root;
        while (frag) {
            next = frag->next;
            free(frag);
            frag = next;
        }

        root = NULL;
        trunk = NULL;
        m_length = 0;
    }
    int StringBuilder::length() {
        return m_length;
    }

    bool StringBuilder::isEmpty() {
        return root == nullptr;
    }

/*
 * sb_append adds a copy of the given string to a StringBuilder.
 */
    StringBuilder &StringBuilder::append(const char *str) {
        int len = 0;
        StringFragment *frag = NULL;

        if (NULL == str || '\0' == *str)
            return *this;

        len = strlen(str);
        frag = (StringFragment *) malloc(sizeof(StringFragment) + (sizeof(char) * len));
        if (frag == nullptr) {
            printf("malloc failed for StringFragment");
            return *this;
        }

        frag->next = NULL;
        frag->length = len;
        memcpy((void *) &frag->str, (const void *) str, sizeof(char) * (len + 1));

        m_length += len;
        if (NULL == root)
            root = frag;
        else
            trunk->next = frag;

        trunk = frag;
        return *this;
    }

/*
 * sb_appendf adds a copy of the given formatted string to a StringBuilder.
 */
    StringBuilder &StringBuilder::appendf(const char *format, ...) {
        if(format == nullptr){
            printf("wrong format");
            return *this;
        }
        int rc = 0;
        char buf[SB_MAX_FRAG_LENGTH];
        va_list args;

        va_start (args, format);
        rc = vsnprintf(&buf[0], SB_MAX_FRAG_LENGTH, format, args);
        va_end(args);

        if (0 > rc)
            return *this;

        return append(buf);
    }

/*
 * sb_concat returns a concatenation of strings that have been appended to the
 * StringBuilder. It is the callers responsibility to free the returned
 * reference.
 *
 * The StringBuilder is not modified by this function and can therefore continue
 * to be used.
 */
    char *StringBuilder::toString() {
        char *buf = NULL;
        char *c = NULL;
        StringFragment *frag = NULL;

        buf = (char *) malloc((m_length + 1) * sizeof(char));
        if (NULL == buf)
            return NULL;

        c = buf;
        for (frag = root; frag; frag = frag->next) {
            memcpy(c, &frag->str, sizeof(char) * frag->length);
            c += frag->length;
        }

        *c = '\0';
        return buf;
    }
    void StringBuilder::toString(std::string &out) {
        StringFragment *frag = NULL;
        for (frag = root; frag; frag = frag->next) {
            out.append(frag->str);
        }
    }
    StringBuilder& StringBuilder::append(void* _val){
        return appendf("%p", _val);
    }
    //--------------- append -----------
#define APPEND(t,type) \
StringBuilder& StringBuilder::append(type _val){ \
    return appendf(m_format[t], _val); \
}
    APPEND(T_BOOL, bool)
    APPEND(T_CHAR, char)
    APPEND(T_U_CHAR, unsigned char)
    APPEND(T_INT, int)
    APPEND(T_U_INT, unsigned int)

    APPEND(T_U_LONG, unsigned long)
    APPEND(T_U_LONG_LONG, unsigned long long)
    APPEND(T_LONG, long)
    APPEND(T_LONG_LONG, long long)

    APPEND(T_FLOAT, float)
    APPEND(T_DOUBLE, double)
    APPEND(T_SHORT, short);
    APPEND(T_U_SHORT, unsigned short);
//--------------------- operators -------------------
    StringBuilder &StringBuilder::operator<<(const char *_val) {
        return append(_val);
    }
    StringBuilder& StringBuilder::operator<<(void* _val) {
        return appendf("%p", _val);
    }

#define OP(t, type) \
StringBuilder& StringBuilder::operator<<(type _val) { \
    return appendf(m_format[t], _val);\
}

    OP(T_BOOL, bool)
    OP(T_CHAR, char)
    OP(T_U_CHAR, unsigned char)
    OP(T_INT, int)
    OP(T_U_INT, unsigned int)

    OP(T_U_LONG, unsigned long)
    OP(T_U_LONG_LONG, unsigned long long)
    OP(T_LONG, long)
    OP(T_LONG_LONG, long long)

    OP(T_FLOAT, float)
    OP(T_DOUBLE, double)
    OP(T_SHORT, short);
    OP(T_U_SHORT, unsigned short);
}