//
// Created by Administrator on 2020/12/7 0007.
//

#ifndef BGFX_STUDY_ACTORLISTENERMANAGER_H
#define BGFX_STUDY_ACTORLISTENERMANAGER_H

#include "../lua/SkRefCnt.h"
#include "utils/Array.h"
#include "../utils/Array.h"
#include "../lua/SkWeakRefCnt.h"

namespace h7{

    class Actor;

    class ActorListener: public SkRefCnt{
    public:
        ITERATOR_CNT_CLASS(ActorListener)
        virtual void onChange(Actor* actor){

        }
    };
    class ActorEventListener: public SkRefCnt{
    public:
        ITERATOR_CNT_CLASS(ActorEventListener)
        virtual void onChange(Actor* actor, unsigned char event){

        }
    };

    class ActorListenerManager{
    public:
        static constexpr unsigned char TYPE_POSITION = 1;
        static constexpr unsigned char TYPE_SIZE = 2;
        static constexpr unsigned char TYPE_SCALE = 3;
        static constexpr unsigned char TYPE_ROTATION = 4;
        static constexpr unsigned char TYPE_TRANSLATE = 5;
    private:
        sk_sp<Actor> actor;

        Array<sk_sp<ActorEventListener>> eventListeners = Array<sk_sp<ActorEventListener>>(0);
        Array<sk_sp<ActorListener>> positions = Array<sk_sp<ActorListener>>(0);
        Array<sk_sp<ActorListener>> sizes = Array<sk_sp<ActorListener>>(0);
        Array<sk_sp<ActorListener>> scales = Array<sk_sp<ActorListener>>(0);
        Array<sk_sp<ActorListener>> rotations = Array<sk_sp<ActorListener>>(0);
        Array<sk_sp<ActorListener>> translates = Array<sk_sp<ActorListener>>(0);

        Array<sk_sp<ActorListener>>& getListeners(unsigned char event);
    public:
        void fire(unsigned char type);

        //weak ref
        inline void setActor(Actor* act){
            actor.reset(act);
        }
        inline void addActorEventListener(sk_sp<ActorEventListener> l){
            eventListeners.add(l);
        }
        inline void removeActorEventListener(sk_sp<ActorEventListener> l){
            eventListeners.remove(l);
        }
        inline void clearActorEventListeners(){
            auto iterator = ActorEventListener::Iterator();
            eventListeners.clear(&iterator);
        }
        inline void clearActorListeners(){
            auto iterator = ActorListener::Iterator();
            positions.clear(&iterator);
            sizes.clear(&iterator);
            scales.clear(&iterator);
            rotations.clear(&iterator);
            translates.clear(&iterator);
        }
        inline void clearActorListeners(unsigned char type){
            auto iterator = ActorListener::Iterator();
            switch (type){
                case TYPE_POSITION:
                    positions.clear(&iterator);
                    break;
                case TYPE_SIZE:
                    sizes.clear(&iterator);
                    break;
                case TYPE_SCALE:
                    scales.clear(&iterator);
                    break;
                case TYPE_ROTATION:
                    rotations.clear(&iterator);
                    break;
                case TYPE_TRANSLATE:
                    translates.clear(&iterator);
                    break;
            }
        }
        inline void addActorListener(unsigned char type,sk_sp<ActorListener> listener){
            switch (type){
                case TYPE_POSITION:
                    positions.add(listener);
                    break;
                case TYPE_SIZE:
                    sizes.add(listener);
                    break;
                case TYPE_SCALE:
                    scales.add(listener);
                    break;
                case TYPE_ROTATION:
                    rotations.add(listener);
                    break;
                case TYPE_TRANSLATE:
                    translates.add(listener);
                    break;
            }
        }
        inline void removeActorListener(unsigned char type,sk_sp<ActorListener> listener){
            switch (type){
                case TYPE_POSITION:
                    positions.remove(listener);
                    break;
                case TYPE_SIZE:
                    sizes.remove(listener);
                    break;
                case TYPE_SCALE:
                    scales.remove(listener);
                    break;
                case TYPE_ROTATION:
                    rotations.remove(listener);
                    break;
                case TYPE_TRANSLATE:
                    translates.remove(listener);
                    break;
            }
        }
    };
}

#endif //BGFX_STUDY_ACTORLISTENERMANAGER_H
