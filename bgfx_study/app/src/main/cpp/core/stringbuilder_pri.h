//
// Created by Administrator on 2020/9/1 0001.
//

#ifndef BGFX_STUDY_STRINGBUILDER_PRI_H
#define BGFX_STUDY_STRINGBUILDER_PRI_H

extern "C" {
    namespace SB {
        typedef struct StringFragment {
            StringFragment *next;
            int length;
            char *str;
            ~StringFragment();
        } StringFragment;
    }
}

#endif //BGFX_STUDY_STRINGBUILDER_PRI_H
