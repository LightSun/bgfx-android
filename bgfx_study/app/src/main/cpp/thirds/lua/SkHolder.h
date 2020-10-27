//
// Created by Administrator on 2020/10/27 0027.
//

#ifndef BGFX_STUDY_SKHOLDER_H
#define BGFX_STUDY_SKHOLDER_H

#define DEF_RESHAPE_TYPE 0  //default unknown type
#define DEF_VALUE 0

#define TYPE_NONE       0
#define TYPE_INT        1
#define TYPE_NUMBER     2
#define TYPE_MEM        3
#define TYPE_MEM_ANY    4
#define TYPE_MEM_MAT    5
#define TYPE_MEM_ARRAY  6
#define TYPE_ERROR_MSG  10

#define MODE_ROW            "r"
#define MODE_COLUMN         "c"
#define MODE_ROW_COLUMN     "rc"   //'r'+'c'
//#define MODE_ROW_REVERSE    "rre"
//#define MODE_COLUMN_REVERSE "cre"
//#define MODE_REVERSE        "re"

class SkHolder {
public:
    char type;
    union {
        double d;
        void *ptr;
        int i;
        const char * msg;
    };
    SkHolder():type(TYPE_NONE){};
    SkHolder(const char type, int i) : type(type), i(i) {}
    SkHolder(char type, double d) : type(type), d(d) {}
    SkHolder(char type, void *ptr) : type(type), ptr(ptr) {}
};

#endif //BGFX_STUDY_SKHOLDER_H
