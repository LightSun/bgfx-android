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
        }
        return *(Array<sk_sp<ActorListener>>*)nullptr;
    }

    void ActorListenerManager::fire(unsigned char event) {
        actor->weak_ref();
        if(actor->try_ref()){
            auto pActor = actor.get();
            for (int i = 0, c = eventListeners.size(); i < c; ++i) {
                eventListeners.get(i)->onChange(pActor, event);
            }
            auto array = getListeners(event);
            if(&array != nullptr){
                for (int i = 0, c = array.size(); i < c; ++i) {
                    array.get(i)->onChange(pActor);
                }
            }
            actor->unref();
        }
        actor->weak_unref();
    }
}