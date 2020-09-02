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

    SB::StringBuilder& getBuffer();

    void prints();
    /**
     * count and end the printer and output.
     * @return null if over count. or a normal str.
     */
    void end();
    void reset();

public:

    /**
     * after call this .you need to free to release chars
     * @tparam Args the arguments type
     * @param fmt the format
     * @param args the real arguments
     * @return the formated string
     */
    template<typename ...Args>
    static const char* format(const char *fmt, Args &&... args) {
        char* cstr;
        int c = snprintf(NULL, 0, fmt, std::forward<Args>(args)...);
        cstr = static_cast<char *>(malloc(c + 1));
        snprintf(cstr, c + 1, fmt, std::forward<Args>(args)...);
        return cstr;
    }

#define PRINT_ARRAY(type) \
static void printArray(type array[], int len, SB::StringBuilder& ss){ \
    ss << "["; \
    for (int i = 0; i < len; ++i) { \
        ss << array[i]; \
        if (i != len - 1) { \
            ss << ","; \
        } \
    } \
    ss << "]"; \
}
    PRINT_ARRAY(float)
    PRINT_ARRAY(uint64_t)
    PRINT_ARRAY(uint32_t)
    PRINT_ARRAY(uint16_t)
    PRINT_ARRAY(uint8_t)
};

#endif //BGFX_STUDY_PRINTER_H
