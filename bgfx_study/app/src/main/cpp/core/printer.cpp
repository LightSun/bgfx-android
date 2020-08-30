//
// Created by Administrator on 2020/8/29 0029.
//

#include "printer.h"
#include <sstream>

#define APPEND(type) \
Printer &Printer::append(type str) { \
    if(alreadyCount < count){ \
        buf << str; \
    } \
    return *this; \
}
//--------------
Printer::Printer(): Printer(-1) {

}
Printer::Printer(int count): count(count), alreadyCount(0) {

}
APPEND(float)
APPEND(double)
APPEND(const char *)
APPEND(unsigned long)
APPEND(int)
APPEND(long)
APPEND(uint8_t)
APPEND(uint16_t)
APPEND(uint32_t)
Printer &Printer::appendArray(float *array, int len) {
    if(alreadyCount < count) {
        printArray<float>(array, len, buf);
    }
    return *this;
}
Printer& Printer::logFunc(Printer::Log _log) {
    this->log = _log;
    return *this;
}
const char * Printer::prints() {
    auto cstr = buf.str().c_str();
    if(log != nullptr){
        log(cstr);
    }
    buf.str("");
    buf.clear();
    return cstr;
}
const char *Printer::end() {
    alreadyCount ++;
    //overflow
    if(count > 0 && alreadyCount > count){
        return NULL;
    }
    return prints();
}
void Printer::reset() {
    buf.str("");
    buf.clear();
    alreadyCount = 0;
}
