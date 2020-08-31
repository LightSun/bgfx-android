//
// Created by Administrator on 2020/8/29 0029.
//

#ifndef BGFX_STUDY_PRINTER_H
#define BGFX_STUDY_PRINTER_H

#include <sstream>

class Printer{

typedef void (*Log)(const char* str);

private:
    std::ostringstream buf;
    /** the call count of end() method. or else return empty string. */
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
        //asprintf: some char will be wrong.
        char* cstr;
        int c = snprintf(NULL, 0, fmt, std::forward<Args>(args)...);
        cstr = new char[ c + 1 ];
        snprintf(cstr, c + 1, fmt, std::forward<Args>(args)...);

        std::string a(cstr);
        free(cstr);
        return cstr;
    }
    template<class T>
    inline static const char* printArray(T* array, int len){
        std::ostringstream ss;
        printArray<T>(array, len, ss);
        auto sr = ss.str();
        std::string a(sr);
        return a.c_str();
    }

    template<class T>
    inline static void printArray(T* array, int len, std::ostringstream& ss){
        ss << "["; //std::string 是可变字符串.
        for (int i = 0; i < len; ++i) {
            ss << array[i];
            if (i != len - 1) {
                ss << ",";
            }
        }
        ss << "]";
        ss.flush();
    }
};


#endif //BGFX_STUDY_PRINTER_H
