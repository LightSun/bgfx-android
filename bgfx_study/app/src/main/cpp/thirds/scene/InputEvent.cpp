//
// Created by Administrator on 2020/11/29 0029.
//

#include "InputEvent.h"
#include "EventListener.h"
#include "Actor.h"
#include "Action.h"
#include "Stage.h"
#include "Group.h"

namespace h7{

    Vector2f & InputEvent::toCoordinates(sk_sp<Actor>& actor, Vector2f actorCoords) {
        actorCoords.set(stageX, stageY);
        actor->stageToLocalCoordinates(actorCoords);
        return actorCoords;
    }
}