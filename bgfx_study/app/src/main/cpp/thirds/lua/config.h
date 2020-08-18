//
// Created by Administrator on 2019/8/13.
//

#ifndef COMMONLUAAPP_CONFIG_H
#define COMMONLUAAPP_CONFIG_H

#ifdef __IOS
#include <ftw.h>
    #define run_cmd(cmd) \
    nftw(cmd, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
#else
#include <malloc.h>
   #define run_cmd(cmd) \
   system(cmd);
#endif

#endif //COMMONLUAAPP_CONFIG_H
