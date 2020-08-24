//
// Created by Administrator on 2020/8/17 0017.
//

#include "global.h"
#include <iostream>

const char* getSuffixStr(const char* fullS, const char* s){
    std::string str(fullS);
    return str.substr(str.rfind(s) + 1).c_str();
}