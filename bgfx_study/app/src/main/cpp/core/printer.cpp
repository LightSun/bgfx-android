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
        printArray(array, len, buf);
    }
    return *this;
}
Printer& Printer::logFunc(Printer::Log _log) {
    this->log = _log;
    return *this;
}
SB::StringBuilder & Printer::getBuffer() {
    return buf;
}
void Printer::prints() {
    auto result = buf.toString();
    if(log != nullptr){
        log(result);
    }
    buf.reset();
    free(result);
}
void Printer::end() {
    alreadyCount ++;
    //overflow
    if(count > 0 && alreadyCount > count){
        return;
    }
    prints();
}
void Printer::reset() {
    buf.reset();
    alreadyCount = 0;
}