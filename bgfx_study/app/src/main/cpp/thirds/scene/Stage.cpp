//
// Created by Administrator on 2020/11/27 0027.
//

#include <limits.h>
#include "Stage.h"
#include "Event.h"
#include "Actor.h"
#include "Group.h"
#include "InputEvent.h"
#include "EventListener.h"


namespace h7{

    void Stage::act(float delta) {
// Update over actors. Done in act() because actors may change position, which can fire enter/exit without an input event.
        for (int pointer = 0, n = TOUCH_NUM; pointer < n; pointer++) {
            sk_sp<Actor> overLast = pointerOverActors[pointer];
            // Check if pointer is gone.
            if (!pointerTouched[pointer]) {
                if (overLast.get() != nullptr) {
                    pointerOverActors[pointer] = nullptr;
                    screenToStageCoordinates(tempCoords.set(pointerScreenX[pointer], pointerScreenY[pointer]));
                    // Exit over last.
                    InputEvent event;
                    event.setType(InputEvent::Type::exit);
                    event.setStage(this);
                    event.setStageX(tempCoords.x);
                    event.setStageY(tempCoords.y);
                    event.setRelatedActor(overLast);
                    event.setPointer(pointer);
                    overLast->fire(event);
                }
                continue;
            }
            // Update over actor for the pointer.
            pointerOverActors[pointer] = fireEnterAndExit(overLast, pointerScreenX[pointer], pointerScreenY[pointer], pointer);
        }
        // Update over actor for the mouse on the desktop.
        //TODO
        /*ApplicationType type = Gdx.app.getType();
        if (type == ApplicationType.Desktop || type == ApplicationType.Applet || type == ApplicationType.WebGL) {
            // if (mouseOverActor != null) mouseOverActor.setDebug(false);
            mouseOverActor = fireEnterAndExit(mouseOverActor, mouseScreenX, mouseScreenY, -1);
            // if (mouseOverActor != null) mouseOverActor.setDebug(true);
        }*/
        root->act(delta);
    }

    Vector2f &Stage::screenToStageCoordinates(Vector2f &f) {
        return f;
    }
    sk_sp<Actor> Stage::fireEnterAndExit(sk_sp<Actor> overLast, int screenX, int screenY,
                                         int pointer) {
// Find the actor under the point.
        screenToStageCoordinates(tempCoords.set(screenX, screenY));
        sk_sp<Actor> over = hit(tempCoords.x, tempCoords.y, true);
        if (over == overLast) return overLast;

        // Exit overLast.
        if (overLast.get() != nullptr) {
            InputEvent event;
            event.setStage(this);
            event.setStageX(tempCoords.x);
            event.setStageY(tempCoords.y);
            event.setPointer(pointer);
            event.setType(InputEvent::Type::exit);
            event.setRelatedActor(over);
            overLast->fire(event);
        }
        // Enter over.
        if (over.get() != nullptr) {
            InputEvent event;
            event.setStage(this);
            event.setStageX(tempCoords.x);
            event.setStageY(tempCoords.y);
            event.setPointer(pointer);
            event.setType(InputEvent::Type::enter);
            event.setRelatedActor(overLast);
            over->fire(event);
        }
        return over;
    }
    sk_sp<Actor> Stage::hit(float stageX, float stageY, bool touchable) {
        root->parentToLocalCoordinates(tempCoords.set(stageX, stageY));
        return root->hit(tempCoords.x, tempCoords.y, touchable);
    }
    bool Stage::touchDown(int screenX, int screenY, int pointer, int button) {
        if(!viewport.isInside(screenX, screenY)){
            return false;
        }
        pointerTouched[pointer] = true;
        pointerScreenX[pointer] = screenX;
        pointerScreenY[pointer] = screenY;

        screenToStageCoordinates(tempCoords.set(screenX, screenY));

        InputEvent event ;
        event.setType(InputEvent::Type::touchDown);
        event.setStage(this);
        event.setStageX(tempCoords.x);
        event.setStageY(tempCoords.y);
        event.setPointer(pointer);
        event.setButton(button);

        sk_sp<Actor> target = hit(tempCoords.x, tempCoords.y, true);
        if (target.get() == nullptr) {
            if (root->touchable == Touchable::enabled){
                root->fire(event);
            }
        } else{
            target->fire(event);
        }
        return event.isHandled();
    }
    bool Stage::touchUp(int screenX, int screenY, int pointer, int button) {
            pointerTouched[pointer] = false;
            pointerScreenX[pointer] = screenX;
            pointerScreenY[pointer] = screenY;

            if (touchFocuses.size() == 0) return false;

            screenToStageCoordinates(tempCoords.set(screenX, screenY));

            InputEvent event;
            event.setType(InputEvent::Type::touchUp);
            event.setStage(this);
            event.setStageX(tempCoords.x);
            event.setStageY(tempCoords.y);
            event.setPointer(pointer);
            event.setButton(button);

            auto array = touchFocuses.copy();
            for (int i = 0, n = array.size(); i < n; i++) {
                sk_sp<TouchFocus> focus = array[i];
                if (focus->pointer != pointer || focus->button != button) continue;
                if (!touchFocuses.remove(focus)) continue; // Touch focus already gone.
                event.setTarget(focus->target);
                event.setListenerActor(focus->listenerActor);
                if (focus->listener->handle(event)){
                    event.handle();
                }
            }
            return event.isHandled();
    }
    bool Stage::touchDragged(int screenX, int screenY, int pointer) {
        pointerScreenX[pointer] = screenX;
        pointerScreenY[pointer] = screenY;
        mouseScreenX = screenX;
        mouseScreenY = screenY;

        if (touchFocuses.size() == 0) return false;

        screenToStageCoordinates(tempCoords.set(screenX, screenY));

        InputEvent event;
        event.setType(InputEvent::Type::touchDragged);
        event.setStage(this);
        event.setStageX(tempCoords.x);
        event.setStageY(tempCoords.y);
        event.setPointer(pointer);

        auto array = touchFocuses.copy();
        for (int i = 0, n = array.size(); i < n; i++) {
            sk_sp<TouchFocus> focus = array[i];
            if (focus->pointer != pointer) continue;
            if (!touchFocuses.contains(focus)) continue; // Touch focus already gone.
            event.setTarget(focus->target);
            event.setListenerActor(focus->listenerActor);
            if (focus->listener->handle(event)){
                event.handle();
            }
        }
        return event.isHandled();
    }
    bool Stage::mouseMoved(int screenX, int screenY) {
        mouseScreenX = screenX;
        mouseScreenY = screenY;

        if (!viewport.isInside(screenX, screenY)) return false;

        screenToStageCoordinates(tempCoords.set(screenX, screenY));

        InputEvent event;
        event.setStage(this);
        event.setType(InputEvent::Type.mouseMoved);
        event.setStageX(tempCoords.x);
        event.setStageY(tempCoords.y);

        sk_sp<Actor> target = hit(tempCoords.x, tempCoords.y, true);
        if (target.get() == nullptr) target = root;

        target->fire(event);
        return event.isHandled();
    }
    bool Stage::scrolled(float amountX, float amountY) {
        sk_sp<Actor> target = scrollFocus.get() == nullptr ? root : scrollFocus;

        screenToStageCoordinates(tempCoords.set(mouseScreenX, mouseScreenY));

        InputEvent event;
        event.setStage(this);
        event.setType(InputEvent::Type::scrolled);
        event.setScrollAmountX(amountX);
        event.setScrollAmountY(amountY);
        event.setStageX(tempCoords.x);
        event.setStageY(tempCoords.y);
        target->fire(event);
        return event.isHandled();
    }
    bool Stage::keyDown(int keyCode) {
        sk_sp<Actor> target = keyboardFocus.get() == nullptr ? root : keyboardFocus;
        InputEvent event;
        event.setStage(this);
        event.setType(InputEvent::Type::keyDown);
        event.setKeyCode(keyCode);
        target->fire(event);
        return event.isHandled();
    }
    bool Stage::keyUp(int keyCode) {
        sk_sp<Actor> target = keyboardFocus.get() == nullptr ? root : keyboardFocus;
        InputEvent event;
        event.setStage(this);
        event.setType(InputEvent::Type::keyUp);
        event.setKeyCode(keyCode);
        target->fire(event);
        return event.isHandled();
    }
    bool Stage::keyTyped(char character) {
        sk_sp<Actor> target = keyboardFocus.get() == nullptr ? root : keyboardFocus;
        InputEvent event;
        event.setStage(this);
        event.setType(InputEvent::Type::keyTyped);
        event.setCharacter(character);
        target->fire(event);
        return event.isHandled();
    }


    void Stage::addTouchFocus(sk_sp<EventListener> listener, sk_sp<Actor> listenerActor, sk_sp<Actor> target, int pointer, int button) {
        sk_sp<TouchFocus> focus = sk_make_sp<TouchFocus>();
        focus->listenerActor = listenerActor;
        focus->target = target;
        focus->listener = listener;
        focus->pointer = pointer;
        focus->button = button;
        touchFocuses.add(focus);
    }
    void Stage::removeTouchFocus(sk_sp<EventListener> listener, sk_sp<Actor> listenerActor, sk_sp<Actor> target, int pointer, int button) {
        for (int i = touchFocuses.size() - 1; i >= 0; i--) {
            sk_sp<TouchFocus> focus = touchFocuses.get(i);
            if (focus->listener == listener && focus->listenerActor == listenerActor && focus->target == target
                && focus->pointer == pointer && focus->button == button) {
                auto sp = touchFocuses.removeAt(i);
                if(sp != nullptr){
                    sp.reset();
                }
            }
        }
    }
    void Stage::cancelTouchFocus(sk_sp<Actor> listenerActor) {
// Cancel all current touch focuses for the specified listener, allowing for concurrent modification, and never cancel the
        // same focus twice.
        sk_sp<InputEvent> event;
        auto array = touchFocuses.copy();
        
        for (int i = 0, n = array.size(); i < n; i++) {
            sk_sp<TouchFocus> focus = array[i];
            if (focus->listenerActor != listenerActor) continue;
            if (!touchFocuses.remove(focus)) continue; // Touch focus already gone.

            if (event.get() == nullptr) {
                event.reset(new InputEvent());
                event->setStage(this);
                event->setType(InputEvent::Type::touchUp);
                event->setStageX(INT_MIN);
                event->setStageY(INT_MIN);
            }

            event->setTarget(focus->target);
            event->setListenerActor(focus->listenerActor);
            event->setPointer(focus->pointer);
            event->setButton(focus->button);
            focus->listener->handle(*event);
            // Cannot return TouchFocus to pool, as it may still be in use (eg if cancelTouchFocus is called from touchDragged).
        }
        event.reset();
    }
}