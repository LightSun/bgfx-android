//
// Created by Administrator on 2020/8/29 0029.
//

#ifndef BGFX_STUDY_PRINTER_H
#define BGFX_STUDY_PRINTER_H

#include "stringbuilder.h"
#include <sstream>

class Printer{

typedef void (*Log)(const char* str);

private:
    //std::ostringstream buf;
    SB::StringBuilder buf;
    /** the call count of end() method. or else return null. */
    int count;
    /** the already count of end */
    int alreadyCount;

    Log log;

public:
    Printer(int count);
    Printer();

    Printer& append(float val);
    Printer& append(double val);
    Printer& append(const char* str);
    Printer& append(uint64_t val);
    Printer& append(long val);
    Printer& append(uint32_t val);
    Printer& append(int val);
    Printer& append(uint8_t val);
    Printer& append(uint16_t val);
    Printer& appendArray(float* array, int len);
    Printer& logFunc(Log log);
    const char * prints();
    /**
     * count and end the printer and output.
     * @return null if over count. or a normal str.
     */
    const char* end();
    void reset();

public:

    template<typename ...Args>
    static const char* format(const char *fmt, Args &&... args) {
        char* cstr;
        int c = snprintf(NULL, 0, fmt, std::forward<Args>(args)...);
        cstr = new char[ c + 1 ];
        snprintf(cstr, c + 1, fmt, std::forward<Args>(args)...);

        std::string a(cstr);
        free(cstr);
        return a.c_str();
    }

    static void printArray(float array[], int len, SB::StringBuilder& ss);
    static void printArray(uint16_t array[], int len, SB::StringBuilder& ss);
    static void printArray(uint8_t  array[], int len, SB::StringBuilder& ss);
    static void printArray(uint32_t array[], int len, SB::StringBuilder& ss);
    static void printArray(uint64_t array[], int len, SB::StringBuilder& ss);
};

#endif //BGFX_STUDY_PRINTER_H
