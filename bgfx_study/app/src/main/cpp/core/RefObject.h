//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_REFOBJECT_H
#define BGFX_STUDY_REFOBJECT_H

#include <atomic>

namespace h7{
    class RefObject{
    public:
        std::atomic_int _ref;

        RefObject(){
            ref();
        }
        ~RefObject(){
            onDestroy();
        }
        virtual int ref(){
            return _ref.fetch_add(1);
        }
        virtual int unRef(){
            return _ref.fetch_add(-1);
        }
        virtual void unRefAndDestroy(){
            if(unRef() == 0){
                delete this;
            }
        }
        virtual void onDestroy(){}
    };
}

#endif //BGFX_STUDY_REFOBJECT_H
