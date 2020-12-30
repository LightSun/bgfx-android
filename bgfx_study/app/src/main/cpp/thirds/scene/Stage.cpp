//
// Created by Administrator on 2020/11/27 0027.
//

#include <limits.h>
#include <log.h>
#include "Stage.h"
#include "Event.h"
#include "Actor.h"
#include "Action.h"
#include "Group.h"
#include "InputEvent.h"
#include "EventListener.h"
#include "FocusListener.h"
#include "../nancanvas/Canvas.h"
#include "input/GestureContext.h"
#include "SceneContext.h"

namespace h7{

    Stage::Stage(h7::Viewport *vp, NanoCanvas::Canvas *canvas) {
        _canvas.reset(canvas);
        viewport.reset(vp);
    }

    Stage::Stage(h7::Viewport *vp, NVGcontext *context) {
        viewport.reset(vp);
        _canvas.reset(new NanoCanvas::Canvas(context, vp->width, vp->height));
    }

    void Stage::draw() {
        if(root->isVisible()){
            root->draw(*_canvas, 1.0f);
        }
    }
    void Stage::layout() {
        if(!_isInLayout){
            _isInLayout = true;
            //if layout range not
            if(_layoutAnchor == nullptr || _layoutRange.isEmpty()){
                root->doLayout(viewport->x, viewport->y, viewport->width, viewport->height);
            } else{
                //1, getAscendants
                Array<sk_sp<Group>> parents;
                if(_layoutAnchor->hasActorType(H7_GROUP_TYPE)){
                    auto pGroup = sk_ref_sp(rCast(Group*, _layoutAnchor.get()));
                    parents.add(pGroup);
                }
                _layoutAnchor->getAscendants(parents);
                SkRect& rect = Actor::_TEMP_RECT;
                //2, second find first not contains layout-range. from root
                //the best match group
                sk_sp<Group> lastMactchGroup;
                for (int i = parents.size() - 1; i >=0 ; i--) {
                    auto sp = parents.get(i);
                    sp->weak_ref();
                    if(sp->try_ref()){
                        rect.setXYWH(sp->getScreenX(), sp->getScreenY(), sp->getWidth(), sp->getHeight());
                        sp->unref();
                    }
                    sp->weak_unref();
                    if(rect.contains(_layoutRange)){
                        lastMactchGroup = sp;
                    } else{
                        if(lastMactchGroup != nullptr){
                            //found the best match parent.
                            break;
                        }
                    }
                }
                //3, now we do layout by target group
                if(lastMactchGroup != nullptr){
                    lastMactchGroup->weak_ref();
                    if(lastMactchGroup->try_ref()){
                        lastMactchGroup->doLayout();
                        lastMactchGroup->unref();
                    }
                    lastMactchGroup->weak_unref();
                } else{
                    LOGE("_layoutRange is not empty, but can't find actor. Couldn't reach here!");
                }
            }
            _layoutAnchor.reset();
            _layoutRange.setEmpty();
            _isInLayout = false;
        }
    }
    void Stage::requestLayout() {
        h7::requestLayout();
    }
    void Stage::invalidate() {
        requestRender();
    }
    bool Stage::isInLayout() {
        return _isInLayout;
    }
    void Stage::setLayoutRect(const h7::Actor *actor, SkRect &in) {
        //reset to full-layout
        if(in.isEmpty()){
            _layoutRange.setEmpty();
            _layoutAnchor.reset();
            return;
        }
        in.normalize(0, 0, viewport->width, viewport->height);
        //if range is smaller
        if(_layoutRange.contains(in)){
            //ignore
            return;
        }
        _layoutAnchor.reset(const_cast<Actor *>(actor));
        _layoutRange.set(in);
        if(_isInLayout){
            _isInLayout = false;
        }
    }
    void Stage::concatLayoutRect(const h7::Actor *actor, SkRect &in) {
        //make valid rect
        if(!_layoutRange.isEmpty()){
#define LAYOUT_RECT_MIN(a)\
    bx::min(_layoutRange.a, in.a)
#define LAYOUT_RECT_MAX(a)\
    bx::max(_layoutRange.a, in.a)
            in.setLTRB(LAYOUT_RECT_MIN(fLeft), LAYOUT_RECT_MIN(fTop),
                                         LAYOUT_RECT_MAX(fRight), LAYOUT_RECT_MAX(fBottom)
            );
        }
        setLayoutRect(actor, in);
    }
    void Stage::act(float delta) {
// Update over actors. Done in act() because actors may change position, which can fire enter/exit without an input event.
        for (int pointer = 0, n = STAGE_TOUCH_NUM; pointer < n; pointer++) {
            sk_sp<Actor> overLast = pointerOverActors[pointer];
            // Check if pointer is gone.
            if (!pointerTouched[pointer]) {
                if (overLast != nullptr) {
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
#if defined(_WIN32) or defined(CCORE_MAC) or defined(CCORE_LINUX)
         //if (mouseOverActor != null) mouseOverActor.setDebug(false);
         mouseOverActor = fireEnterAndExit(mouseOverActor, mouseScreenX, mouseScreenY, -1);
         //if (mouseOverActor != null) mouseOverActor.setDebug(true);
#endif
        root->act(delta);
    }

    Vector2f &Stage::screenToStageCoordinates(Vector2f &f) {
        return f.sub(viewport->x, viewport->y);
    }
    Vector2f& Stage::stageToScreenCoordinates(h7::Vector2f &f) {
        return f.add(viewport->x, viewport->y);
    }
    sk_sp<Actor> Stage::fireEnterAndExit(sk_sp<Actor>& overLast, int screenX, int screenY,
                                         int pointer) {
// Find the actor under the point.
        screenToStageCoordinates(tempCoords.set(screenX, screenY));
        sk_sp<Actor> over = sk_ref_sp(hit(tempCoords.x, tempCoords.y, true));
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
    const Actor* Stage::hit(float stageX, float stageY, bool touchable) {
        root->parentToLocalCoordinates(tempCoords.set(stageX, stageY));
        return root->hit(tempCoords.x, tempCoords.y, touchable);
    }
    bool Stage::touchDown(int screenX, int screenY, int pointer, int button) {
        if(!viewport->isInside(screenX, screenY)){
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

        Actor* target = const_cast<Actor *>(hit(tempCoords.x, tempCoords.y, true));
        if (target == nullptr) {
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

        if (!viewport->isInside(screenX, screenY)) return false;

        screenToStageCoordinates(tempCoords.set(screenX, screenY));

        InputEvent event;
        event.setStage(this);
        event.setType(InputEvent::Type::mouseMoved);
        event.setStageX(tempCoords.x);
        event.setStageY(tempCoords.y);

        sk_sp<Actor> target = sk_ref_sp(hit(tempCoords.x, tempCoords.y, true));
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


    void Stage::addTouchFocus(const EventListener* listener, const Actor* listenerActor,
                              const Actor* target, int pointer, int button) {
        sk_sp<TouchFocus> focus = sk_make_sp<TouchFocus>();
        focus->listenerActor = sk_ref_sp(listenerActor);
        focus->target = sk_ref_sp(target);
        focus->listener = sk_ref_sp(listener);
        focus->pointer = pointer;
        focus->button = button;
        touchFocuses.add(focus);
    }
    void Stage::removeTouchFocus(const EventListener* listener, const Actor* listenerActor,
                                 const Actor* target, int pointer, int button) {
        for (int i = touchFocuses.size() - 1; i >= 0; i--) {
            sk_sp<TouchFocus> focus = touchFocuses.get(i);
            if (focus->listener.get() == listener && focus->listenerActor.get() == listenerActor && focus->target.get() == target
                && focus->pointer == pointer && focus->button == button) {
                auto sp = touchFocuses.removeAt(i);
                if(sp != nullptr){
                    sp.reset();
                }
            }
        }
    }
    void Stage::cancelTouchFocus(const Actor* listenerActor) {
// Cancel all current touch focuses for the specified listener, allowing for concurrent modification, and never cancel the
        // same focus twice.
        sk_sp<InputEvent> event;
        auto array = touchFocuses.copy();
        
        for (int i = 0, n = array.size(); i < n; i++) {
            sk_sp<TouchFocus> focus = array[i];
            if (focus->listenerActor.get() != listenerActor) continue;
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

    void Stage::addAction(Action* action) {
        root->addAction(action);
    }
    void Stage::addActor(h7::Actor *actor) {
        root->addActor(actor);
    }
    Array<sk_sp<Actor>>& Stage::getActors() {
        return root->children;
    }
    void Stage::unfocus(Actor *actor) {
        cancelTouchFocus(actor);
        if (scrollFocus != nullptr && scrollFocus->isDescendantOf(actor)) setScrollFocus(nullptr);
        if (keyboardFocus != nullptr && keyboardFocus->isDescendantOf(actor)) setKeyboardFocus(nullptr);
    }
    bool Stage::setScrollFocus(Actor *actor) {
        if (scrollFocus.get() == actor) return true;
        FocusEvent event;
        event.setStage(this);
        event.setType(FocusEvent::Type::scroll);
        Actor* oldScrollFocus = scrollFocus.get();
        if (oldScrollFocus != nullptr) {
            event.setFocused(false);
            event.setRelatedActor(actor);
            oldScrollFocus->fire(event);
        }
        bool success = !event.isCancelled();
        if (success) {
            auto sp = sk_ref_sp(oldScrollFocus);
            scrollFocus.reset(actor);
            if (actor != nullptr) {
                event.setFocused(true);
                event.setRelatedActor(oldScrollFocus);
                actor->fire(event);
                success = !event.isCancelled();
                if (!success){
                    scrollFocus = sp;
                }
            }
        }
        return success;
    }
    bool Stage::setKeyboardFocus(h7::Actor *actor) {
        if (keyboardFocus.get() == actor) return true;
        FocusEvent event;
        event.setStage(this);
        event.setType(FocusEvent::Type::keyboard);
        Actor* oldKeyboardFocus = keyboardFocus.get();
        if (oldKeyboardFocus != nullptr) {
            event.setFocused(false);
            event.setRelatedActor(actor);
            oldKeyboardFocus->fire(event);
        }
        bool success = !event.isCancelled();
        if (success) {
            auto sp = sk_ref_sp(oldKeyboardFocus);
            keyboardFocus.reset(actor);
            if (actor != nullptr) {
                event.setFocused(true);
                event.setRelatedActor(oldKeyboardFocus);
                actor->fire(event);
                success = !event.isCancelled();
                if (!success) {
                    keyboardFocus = sp;
                }
            }
        }
        return success;
    }

    void Stage::setRoot(h7::Group *root) {
        auto parent = root->getParent();
        if (parent != nullptr){
            parent->removeActor(root, false);
        }
        this->root.reset(root);
        root->setParent(nullptr);
        root->setStage(this);
    }

}