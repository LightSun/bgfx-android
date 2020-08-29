//
// Created by Administrator on 2020/8/29 0029.
//

#ifndef BGFX_STUDY_PRINTER_H
#define BGFX_STUDY_PRINTER_H

#include <sstream>

class Printer{

private:
    std::ostringstream buf;

public:
    Printer& append(const char* str);
    Printer& append(uint64_t val);
    Printer& append(long val);
    Printer& append(uint32_t val);
    Printer& append(int val);
    Printer& append(uint8_t val);
    Printer& append(uint16_t val);
    Printer& appendArray(float* array, int len);
    const char* end();

public:
    inline static const char* printFloatArray(float* array, int len);
    inline static void printFloatArray(float* array, int len, std::ostringstream& ss);
};


#endif //BGFX_STUDY_PRINTER_H
