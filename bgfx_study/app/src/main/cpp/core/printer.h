//
// Created by Administrator on 2020/8/29 0029.
//

#ifndef BGFX_STUDY_PRINTER_H
#define BGFX_STUDY_PRINTER_H

#include <sstream>

class Printer{

private:
    std::ostringstream buf;
    /** the call count of end() method. or else return empty string. */
    int count;
    /** the already count of end */
    int alreadyCount;

public:
    Printer(int count);
    Printer();

    Printer& append(const char* str);
    Printer& append(uint64_t val);
    Printer& append(long val);
    Printer& append(uint32_t val);
    Printer& append(int val);
    Printer& append(uint8_t val);
    Printer& append(uint16_t val);
    Printer& appendArray(float* array, int len);
    /**
     * end the printer and output.
     * @param Log the log function. can called multi
     * @param maxLen the max len of one log. this is useful when you want to split-logs.
     * @return null if over count. or a normal str.
     */
    const char* end(void (*Log)(const char* str), int maxLen);
    void reset();

public:
    template<class T>
    inline static const char* printArray(T* array, int len){
        std::ostringstream ss;
        printArray<T>(array, len, ss);
        return ss.str().c_str();
    }

    template<class T>
    inline static void printArray(T* array, int len, std::ostringstream& ss){
        ss << "[";
        for (int i = 0; i < len; ++i) {
            ss << array[i];
            if (i != len - 1) {
                ss << ",";
            }
        }
        ss << "]";
    }

    template<typename T, typename... Args>
    static std::ostringstream & mprintf(std::ostringstream & ostr,
                           const char * fstr, const T & x) throw()
    {
        size_t i=0;
        char c = fstr[0];

        while (c != '%')
        {
            if(c == 0) return ostr; // string is finished
            ostr << c;
            c = fstr[++i];
        };
        c = fstr[++i];
        ostr << x;

        if(c==0) return ostr; //

        // print the rest of the stirng
        ostr << &fstr[++i];
        return ostr;
    }
    template<typename T, typename... Args>
    static std::ostringstream & mprintf(std::ostringstream & ostr,
                           const char * fstr, const T & x, Args... args) throw()
    {
        size_t i=0;
        char c = fstr[0];

        while (c != '%')
        {
            if(c == 0) return ostr; // string is finished
            ostr << c;
            c = fstr[++i];
        };
        c = fstr[++i];
        ostr << x;

        if(c==0) return ostr; // string is finished

        return mprintf(ostr, &fstr[++i], args...);
    }
    /** example
     * int main()
{
    int c = 50*6;
    double a = 34./67.;
    std::string q = "Hello!";

    // put only two arguments
    // the symbol after % does not matter at all
    mprintf(std::cout, "%f + %f = %a \n", c, a);

    // print string object: for real printf one should write q.c_str()
    mprintf(std::cout, "message: \"%s\". \n", q);

    // the last argument will be ignored
    mprintf(std::cout, "%z + %f\n", (long)a, 12, 544 );

}
     */
};


#endif //BGFX_STUDY_PRINTER_H
