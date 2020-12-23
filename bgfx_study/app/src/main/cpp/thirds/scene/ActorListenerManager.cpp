//
// Created by Administrator on 2020/12/7 0007.
//

#include "ActorListenerManager.h"
#include "Actor.h"
#include "Action.h"
#include "Stage.h"
#include "Group.h"
#include "EventListener.h"
#include "Event.h"

namespace h7{
    Array<sk_sp<ActorListener>>& ActorListenerManager::getListeners(unsigned char event) {
        switch (event){
            case ActorListenerManager::TYPE_POSITION:
                return positions;
            case ActorListenerManager::TYPE_ROTATION:
                return rotations;
            case ActorListenerManager::TYPE_SCALE:
                return scales;
            case ActorListenerManager::TYPE_SIZE:
                return sizes;
            case ActorListenerManager::TYPE_TRANSLATE:
                return translates;
            case ActorListenerManager::TYPE_ALPHA:
                return alphas;
        }
        return *(Array<sk_sp<ActorListener>>*)nullptr;
    }

    void ActorListenerManager::fire(unsigned char event) {
        actor->weak_ref();
        if(actor->try_ref()){
            auto pActor = actor.get();
            for (int i = 0, c = eventListeners.size(); i < c; ++i) {
                eventListeners.get(i)->onChange(pActor, preInfo, event);
            }
            auto array = getListeners(event);
            if(&array != nullptr){
                for (int i = 0, c = array.size(); i < c; ++i) {
                    array.get(i)->onChange(pActor, preInfo);
                }
            }
            actor->unref();
        }
        actor->weak_unref();
    }
    void ActorListenerManager::preFire(unsigned char event) {
        actor->weak_ref();
        if(actor->try_ref()){
            switch (event){
                case ActorListenerManager::TYPE_POSITION:
                    preInfo.x = actor->getX();
                    preInfo.y = actor->getY();
                    break;
                case ActorListenerManager::TYPE_ROTATION:
                case ActorListenerManager::TYPE_SCALE:
                case ActorListenerManager::TYPE_TRANSLATE:
                    preInfo.m.set(actor->getMatrix());
                    break;
                case ActorListenerManager::TYPE_SIZE:
                    preInfo.width = actor->getWidth();
                    preInfo.height = actor->getHeight();
                    break;

                case ActorListenerManager::TYPE_ALPHA:
                    preInfo.alpha = actor->getAlpha();
                    break;
            }
            //saveInfo(actor.get(), preInfo);
            actor->unref();
        }
        actor->weak_unref();
    }
    void ActorListenerManager::saveInfo(h7::Actor *actor, h7::ActorInfo &info) {
        info.x = actor->getX();
        info.y = actor->getY();
        info.width = actor->getWidth();
        info.width = actor->getHeight();
        info.alpha = actor->getAlpha();
        info.m.set(actor->getMatrix());
    }
}