/*
 * idea from - https://github.com/cavaliercoder/c-stringbuilder
 * writer: Heaven7
 */

#ifndef SB_H
#define SB_H

#include <string>
#include <map>

namespace SB{
#ifndef SB_MAX_FRAG_LENGTH
#define SB_MAX_FRAG_LENGTH	2048
#endif

#define T_BOOL 1
#define T_INT 2
#define T_CHAR 3
#define T_U_CHAR 4
#define T_U_INT 5
#define T_LONG 6
#define T_U_LONG 7
#define T_LONG_LONG 8
#define T_U_LONG_LONG 9
#define T_FLOAT 10
#define T_DOUBLE 11
#define T_SHORT 12
#define T_U_SHORT 13

struct StringFragment;

class StringBuilder{
public:
    /**
     * create string builder. which can control format of output,
     * @param formatMap  the format define map
     */
    StringBuilder(std::map<unsigned char,const char*>& formatMap);
    /**
   * create string builder with default formats
   */
    StringBuilder();
    ~StringBuilder();

    bool isEmpty();
    StringBuilder& appendf(const char *format, ...);
    StringBuilder& append(const char *str);
    StringBuilder& append(void* _val);

    StringBuilder& append(bool _val);
    StringBuilder& append(int _val);
    StringBuilder& append(char _val);
    StringBuilder& append(unsigned char _val);
    StringBuilder& append(unsigned int _val);
    StringBuilder& append(long _val);
    StringBuilder& append(long long _val);
    StringBuilder& append(unsigned long long _val);
    StringBuilder& append(unsigned long _val);
    StringBuilder& append(float _val);
    StringBuilder& append(double _val);
    StringBuilder& append(short _val);
    StringBuilder& append(unsigned short _val);

    StringBuilder& append(StringBuilder& other);

    /**
     * you need to release the char*
     * @return the string
     */
    char* toString();
    void toString(std::string& out);
    void reset();
    int length();

    StringBuilder& operator << (void* _val);
    StringBuilder& operator << (const char* _val);

    StringBuilder& operator << (bool _val);
    StringBuilder& operator << (int _val);
    StringBuilder& operator << (char _val);
    StringBuilder& operator << (unsigned char _val);
    StringBuilder& operator << (unsigned int _val);
    StringBuilder& operator << (long _val);
    StringBuilder& operator << (long long _val);
    StringBuilder& operator << (unsigned long long _val);
    StringBuilder& operator << (unsigned long _val);
    StringBuilder& operator << (float _val);
    StringBuilder& operator << (double _val);

    StringBuilder& operator <<(short _val);
    StringBuilder& operator <<(unsigned short _val);
    StringBuilder& operator <<(StringBuilder& other);

private:
    StringFragment* root;
    StringFragment* trunk;
    int m_length;

    std::map<unsigned char,const char*> m_format;
};
}

#endif