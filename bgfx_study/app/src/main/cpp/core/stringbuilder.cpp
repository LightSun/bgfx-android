#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "stringbuilder.h"
#include "stringbuilder_pri.h"
#include "SkUTF.h"
#include "log.h"

namespace SB {
    static const std::map<unsigned char, const char *> _DEF_FORMATS = {
            {T_BOOL,        "%d"},
            {T_CHAR,        "%d"},
            {T_U_CHAR,      "%d"},
            {T_U_INT,       "%u"},
            {T_INT,         "%d"},
            {T_LONG,        "%ld"},
            {T_LONG_LONG,   "%lld"},
            {T_U_LONG,      "%lu"},
            {T_U_LONG_LONG, "%llu"},
            {T_FLOAT,       "%g"}, //%g used to remove the suffix zero. this is useful for float and double.
            {T_DOUBLE,      "%g"},
            {T_SHORT,       "%d"},
            {T_U_SHORT,     "%u"},
    };
#define _D_MAP (const_cast<std::map<unsigned char, const char *> &>(_DEF_FORMATS))

#define _COPY_VAL_IF_NEED(type) \
if(formatMap[type] == nullptr){\
    m_format[type] = _D_MAP[type];\
}

    StringBuilder::StringBuilder() : StringBuilder(_D_MAP) {
    }

    StringBuilder::StringBuilder(std::map<unsigned char, const char *> &formatMap) : root(nullptr),
                                                                                     trunk(nullptr),
                                                                                     m_length(0) {
        m_format = formatMap;
        //copy default if doesn't have
        if(formatMap != _D_MAP){
            _COPY_VAL_IF_NEED(T_BOOL);
            _COPY_VAL_IF_NEED(T_CHAR);
            _COPY_VAL_IF_NEED(T_U_CHAR);
            _COPY_VAL_IF_NEED(T_U_INT);
            _COPY_VAL_IF_NEED(T_INT);
            _COPY_VAL_IF_NEED(T_LONG);
            _COPY_VAL_IF_NEED(T_LONG_LONG);
            _COPY_VAL_IF_NEED(T_U_LONG);
            _COPY_VAL_IF_NEED(T_U_LONG_LONG);
            _COPY_VAL_IF_NEED(T_FLOAT);
            _COPY_VAL_IF_NEED(T_DOUBLE);
            _COPY_VAL_IF_NEED(T_SHORT);
            _COPY_VAL_IF_NEED(T_U_SHORT);
        }
    }

    StringBuilder::~StringBuilder() {
        reset();
    }

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

    StringFragment::~StringFragment() {
        free(str);
    }

    StringBuilder &StringBuilder::append(const char *str) {
        int len = 0;
        StringFragment *frag = NULL;
        if (NULL == str || '\0' == *str)
            return *this;

        len = strlen(str);
        frag = new StringFragment();

        frag->next = NULL;
        frag->length = len;
        frag->str = static_cast<char *>(malloc(len + 1));
        //strcpy(frag->str, str);
         memcpy((void *) frag->str, (const void *) str, sizeof(char) * (len + 1));

        m_length += len;
        if (NULL == root)
            root = frag;
        else
            trunk->next = frag;

        trunk = frag;
        return *this;
    }

    StringBuilder &StringBuilder::appendf(const char *format, ...) {
        if (format == nullptr) {
            printf("wrong format");
            return *this;
        }
        int rc = 0;
        char buf[SB_MAX_FRAG_LENGTH] = {0}; //'\0'
        va_list args;

        va_start (args, format);
        rc = vsnprintf(&buf[0], SB_MAX_FRAG_LENGTH, format, args);
        va_end(args);

        if (0 > rc)
            return *this;

        return append(buf);
    }

    char *StringBuilder::toString() {
        char *buf = NULL;
        char *c = NULL;
        StringFragment *frag = NULL;

        buf = (char *) malloc((m_length + 1) * sizeof(char));
        if (NULL == buf)
            return NULL;

        c = buf;
        for (frag = root; frag; frag = frag->next) {
           // strncpy(c, frag->str, frag->length);
            memcpy(c, frag->str, sizeof(char) * frag->length);
            c += frag->length;
        }

        *c = '\0';
        return buf;
    }

    StringBuilder &StringBuilder::append(void *_val) {
        return appendf("%p", _val);
    }

    StringBuilder &StringBuilder::append(StringBuilder &other) {
        const char* str = other.toString();
        append(str);
        //malloc / calloc / realloc
        free((void *) str);
        return *this;
    }
    //--------------- append -----------
#define APPEND(t, type) \
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

    StringBuilder &StringBuilder::operator<<(void *_val) {
        return appendf("%p", _val);
    }

    StringBuilder &StringBuilder::operator<<(StringBuilder &other) {
        return append(other);
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