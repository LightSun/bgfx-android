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
const char *Printer::end(void (*Log)(const char* str), int maxLen) {
    alreadyCount ++;
    //overflow
    if(count > 0 && alreadyCount > count){
        return NULL;
    }
    auto result = buf.str();
    buf.clear();
    if(Log != nullptr && result.length() > maxLen){
        int mod = result.length() % maxLen ;
        int size = mod > 0 ? result.length() / maxLen + 1 : result.length() / maxLen;
        long start = 0;
        bool enough;
        for (int i = 0; i < size; ++i) {
            enough = (result.length() - start) >= maxLen;
            if(enough){
                Log(result.substr(start, start + maxLen).c_str());
            } else{
                Log(result.substr(start).c_str());
            }
            start += maxLen;
        }
    }
    return result.c_str();
}
void Printer::reset() {
    alreadyCount = 0;
}
