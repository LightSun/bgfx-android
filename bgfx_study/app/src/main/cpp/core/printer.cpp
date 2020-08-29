//
// Created by Administrator on 2020/8/29 0029.
//

#include "printer.h"
#include <sstream>

const char *Printer::printFloatArray(float *array, int len) {
    std::ostringstream ss;
    printFloatArray(array, len, ss);
    return ss.str().c_str();
}

void Printer::printFloatArray(float *array, int len, std::ostringstream &ss) {
    ss << "[";
    for (int i = 0; i < len; ++i) {
        ss << array[i];
        if (i != len - 1) {
            ss << ",";
        }
    }
    ss << "]";
}

//--------------
Printer &Printer::append(const char *str) {
    buf << str;
    return *this;
}
Printer &Printer::appendArray(float *array, int len) {
    printFloatArray(array, len, buf);
    return *this;
}

Printer & Printer::append(unsigned long val) {
    buf << val;
    return *this;
}
Printer & Printer::append(int val) {
    buf << val;
    return *this;
}
Printer & Printer::append(long val) {
    buf << val;
    return *this;
}
Printer & Printer::append(uint8_t val) {
    buf << val;
    return *this;
}
Printer & Printer::append(uint16_t val) {
    buf << val;
    return *this;
}
Printer & Printer::append(uint32_t val) {
    buf << val;
    return *this;
}

const char *Printer::end() {
    auto result = buf.str().c_str();
    buf.clear();
    return result;
}

