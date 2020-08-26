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

#ifdef __USE_PTHREAD
#include "pthread.h"
#define lua_lock(L) pthread_mutex_lock(&(G(L)->lock));
#define lua_unlock(L) pthread_mutex_unlock(&(G(L)->lock));
#else
/*
** macros that are executed whenever program enters the Lua core
** ('lua_lock') and leaves the core ('lua_unlock')
*/
// let you self-define lua_lock and lua_unlock
#define lua_lock(L)	((void) 0)
#define lua_unlock(L)	((void) 0)
#endif

#endif //COMMONLUAAPP_CONFIG_H
