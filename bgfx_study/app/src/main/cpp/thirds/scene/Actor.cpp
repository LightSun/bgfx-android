//
// Created by Administrator on 2020/11/27 0027.
//
#include <math.h>
#include <log.h>
#include "bx/math.h"
#include "Drawable.h"
#include "Actor.h"
#include "Stage.h"
#include "Action.h"
#include "Group.h"
#include "Event.h"
#include "EventListener.h"
#include "lua/SkWeakRefCnt.h"
#include "../input/GestureContext.h"
#include "ScrollListener.h"

#include "SceneHelper.h"
#include "nancanvas/Canvas.h"
#include "nancanvas/SkRect.h"

namespace h7 {
    SkRect Actor::_TEMP_RECT;

    Actor::Actor():SkWeakRefCnt(), _scrollInfo(nullptr), _margin(nullptr), _padding(nullptr) {
        _actorListenerM.setActor(this);
    }
    Actor::~Actor(){
        DESTROY_POINTER(_padding);
        DESTROY_POINTER(_margin);
        DESTROY_POINTER(_scrollInfo);
    }
    void Actor::setBackground(Drawable *d) {
        background.reset(d);
    }
    Drawable* Actor::getBackground() {
        return background.get();
    }
    void Actor::setPadding(float left, float top, float right, float bottom) {
        if(_padding == nullptr){
            _padding = new SkRect{left, top, right, bottom};
        } else{
            _padding->setLTRB(left, top, right, bottom);
        }
    }
    SkRect* Actor::getPadding() {
        return _padding;
    }
    SkRect & Actor::getPadding(SkRect &out) {
        if(_padding != nullptr){
            out.set(*_padding);
        } else{
            out.setLTRB(0, 0, 0, 0);
        }
        return out;
    }
    SkRect & Actor::getMargin(SkRect &out) {
        if(_margin != nullptr){
            out.set(*_margin);
        } else{
            out.setLTRB(0, 0, 0, 0);
        }
        return out;
    }
    void Actor::setMargin(float left, float top, float right, float bottom) {
        if(_margin == nullptr){
            _margin = new SkRect{left, top, right, bottom};
        } else{
            _margin->setLTRB(left, top, right, bottom);
        }
    }
    SkRect* Actor::getMargin() {
        return _margin;
    }
    //----------------------------------------------------
    void Actor::requestLayoutAndInvalidate() {
        if (stage != nullptr) {
            stage->weak_ref();
            if (stage->try_ref()) {
                stage->requestLayout();
                stage->invalidate();
                stage->unref();
            }
            stage->weak_unref();
        }
    }
    void Actor::invalidate() {
        if (stage != nullptr) {
            stage->weak_ref();
            if (stage->try_ref()) {
                stage->invalidate();
                stage->unref();
            }
            stage->weak_unref();
        }
    }
    void Actor::computeScroll() {

    }

    Stage *Actor::getStage() {
        if (stage == nullptr)return nullptr;
        stage->weak_ref();
        Stage *result;
        if (stage->try_ref()) {
            result = stage.get();
            stage->unref();
        } else { result = nullptr; }
        stage->weak_unref();
        return result;
    }

    Group *Actor::getParent() {
        if (parent == nullptr)return nullptr;
        parent->weak_ref();
        Group *result;
        if (parent->try_ref()) {
            result = parent.get();
            parent->unref();
        } else { result = nullptr; }
        parent->weak_unref();
        return result;
    }
    // DEFINE_GET_WEAK_REF_METHOD2(Actor::, Group, Parent, parent)
    bool Actor::isInLayout() {
        auto pStage = getStage();
        return pStage != nullptr && pStage->isInLayout();
    }
    void Actor::draw(NanoCanvas::Canvas &canvas, float parentAlpha) {
        if(isVisible() && width > 0 && height > 0){
            computeScroll();
            background->getBounds().setXYWH(0, 0, width, height);
            background->setPadding(_padding);
            if (_mat.isIdentity()) {
                background->draw(canvas, tmpScreenX, tmpScreenY, width, height);
                onDraw(canvas, parentAlpha);
            } else {
                canvas.save();
                canvas.setMatrix(_mat);
                background->draw(canvas, tmpScreenX, tmpScreenY, width, height);
                onDraw(canvas, parentAlpha);
                canvas.restore();
            }
            background->setPadding(nullptr);
        }
    }

    void Actor::act(float delta) {
        if (actions.size() == 0) return;

        auto pStage = getStage();
        if (pStage != NULL) {
            if (pStage->getActionsRequestRendering()) {
                requestRender();
            }
        }
        for (int i = 0; i < actions.size(); i++) {
            sk_sp<Action> action = actions.get(i);
            if (action->act(delta) && i < actions.size()) {
                sk_sp<Action> current = actions.get(i);
                int actionIndex = current == action ? i : actions.indexOf(action);
                if (actionIndex != -1) {
                    actions.removeAt(actionIndex);
                    action->setActor(NULL);
                    i--;
                }
            }
        }
    }

    bool Actor::fire(Event &event) {
        if (event.getStage() == NULL) {
            event.setStage(stage);
        }
        event.setTarget(this);

        // Collect ascendants so event propagation is unaffected by hierarchy changes.
        Array<sk_sp<Group>> ascendants;
        getAscendants(ascendants);

        // Notify ascendants' capture listeners, starting at the root. Ascendants may stop an event before children receive it.
        // Array<sk_sp<Group>>& ascendantsArray = ascendants.copy();
        for (int i = ascendants.size() - 1; i >= 0; i--) {
            sk_sp<Group> currentTarget = ascendants[i];
            currentTarget->notify(event, true);
            if (event.isStopped()) return event.isCancelled();
        }

        // Notify the target capture listeners.
        notify(event, true);
        if (event.isStopped()) return event.isCancelled();

        // Notify the target listeners.
        notify(event, false);
        if (!event.getBubbles()) return event.isCancelled();
        if (event.isStopped()) return event.isCancelled();

        // Notify ascendants' actor listeners, starting at the target. Children may stop an event before ascendants receive it.
        for (int i = 0, n = ascendants.size(); i < n; i++) {
            ascendants[i]->notify(event, false);
            if (event.isStopped()) return event.isCancelled();
        }
        return event.isCancelled();
    }

    bool Actor::notify(Event &event, bool capture) {
        if (event.getTarget() == NULL) {
            return false;
        }

        Array<sk_sp<EventListener>> &listeners = capture ? captureListeners : this->listeners;
        if (listeners.size() == 0) return event.isCancelled();

        event.setListenerActor(this);
        event.setCapture(capture);
        if (event.getStage() == NULL) event.setStage(stage);

        auto array = listeners.copy();
        for (int i = 0, n = array.size(); i < n; i++) {
            if (array[i]->handle(event)) {
                event.handle();
            }
        }
        return event.isCancelled();
    }
    void Actor::getAscendants(Array<sk_sp<Group>>& ascendants) {
        // Collect ascendants so event propagation is unaffected by hierarchy changes.
        parent->weak_ref();
        if (parent->try_ref()) {
            sk_sp<Group> _parent = this->parent;
            while (_parent != NULL) {
                ascendants.add(_parent);
                //
                auto pGroup = _parent->parent;
                pGroup->weak_ref();
                if (pGroup->try_ref()) {
                    _parent = pGroup;
                    pGroup->unref();
                } else {
                    _parent = nullptr;
                }
                pGroup->weak_unref();
            }
            parent->unref();
        }
        parent->weak_unref();
    }

    const Actor *Actor::hit(float x, float y, bool touchable) {
        if (touchable && this->touchable != Touchable::enabled) return NULL;
        if (!isVisible()) return NULL;
        return (x >= 0 && x < width && y >= 0 && y < height ? this : NULL);
    }

    Vector2f &Actor::screenToLocalCoordinates(Vector2f &screenCoords) {
        auto pStage = getStage();
        if (pStage == nullptr) return screenCoords;
        return stageToLocalCoordinates(pStage->screenToStageCoordinates(screenCoords));
    }

    Vector2f &Actor::localToScreenCoordinates(h7::Vector2f &vec) {
        localCoordinatesToStage(vec);
        auto pStage = getStage();
        if (pStage != nullptr) {
            pStage->stageToScreenCoordinates(vec);
        }
        return vec;
    }

    Vector2f &Actor::stageToLocalCoordinates(Vector2f &stageCoords) {
        if (parent != nullptr) {
            parent->weak_ref();
            if (parent->try_ref()) {
                parent->stageToLocalCoordinates(stageCoords);
                parent->unref();
            }
            parent->weak_unref();
        }
        parentToLocalCoordinates(stageCoords);
        return stageCoords;
    }

    Vector2f &Actor::localCoordinatesToStage(h7::Vector2f &vec) {
        localToParentCoordinates(vec);
        if (parent != nullptr) {
            parent->weak_ref();
            if (parent->try_ref()) {
                parent->localToParentCoordinates(vec);
                parent->unref();
            }
            parent->weak_unref();
        }
        return vec;
    }

    Vector2f &Actor::localToParentCoordinates(h7::Vector2f &localCoords) {
        localCoords.x += x;
        localCoords.y += y;
        return localCoords;
    }

    Vector2f &Actor::parentToLocalCoordinates(Vector2f &parentCoords) {
        parentCoords.x -= x;
        parentCoords.y -= y;
        return parentCoords;
    }

    bool Actor::removeFromParent() {
        auto pGroup = getParent();
        if (pGroup != nullptr) {
            return pGroup->removeActor(this, true);
        }
        return false;
    }

    void Actor::clearActions() {
        auto it = Action::Iterator();
        actions.clear(&it);
    }

    void Actor::clearListeners() {
        auto it = EventListener::Iterator();
        listeners.clear(&it);
        captureListeners.clear(&it);
    }

    bool Actor::isDescendantOf(Actor *actor) {
        if (actor != nullptr) {
            Actor *_parent = this;
            do {
                if (_parent == actor) return true;
                _parent = _parent->getParent();
            } while (_parent != nullptr);
        }
        return false;
    }

    bool Actor::isAscendantOf(Actor *actor) {
        if (actor != nullptr) {
            Actor *act = actor;
            do {
                if (act == this) return true;
                act = act->getParent();
            } while (act != nullptr);
        }
        return false;
    }

    bool Actor::hasKeyboardFocus() {
        Stage *stage = getStage();
        return stage != nullptr && stage->getKeyboardFocus() == this;
    }

    bool Actor::hasScrollFocus() {
        Stage *stage = getStage();
        return stage != nullptr && stage->getScrollFocus() == this;
    }

    bool Actor::isTouchFocusTarget() {
        Stage *stage = getStage();
        if (stage == nullptr) return false;
        for (int i = 0, n = stage->touchFocuses.size(); i < n; i++)
            if (stage->touchFocuses.get(i)->target.get() == this) return true;
        return false;
    }

    bool Actor::isTouchFocusListener() {
        Stage *stage = getStage();
        if (stage == nullptr) return false;
        for (int i = 0, n = stage->touchFocuses.size(); i < n; i++)
            if (stage->touchFocuses.get(i)->listenerActor.get() == this) return true;
        return false;
    }

    bool Actor::setZIndex(int index) {
        if (index < 0) return false;
        Group *parent = getParent();
        if (parent == nullptr) return false;
        Array<sk_sp<Actor>> children = parent->children;
        if (children.size() == 1) return false;
        index = bx::min(index, children.size() - 1);
        if (children.get(index).get() == this) return false;

        auto sp = sk_ref_sp<Actor>(this);
        if (!children.remove(sp)) return false;
        children.add(index, sp);
        return true;
    }

    int Actor::getZIndex() {
        Group *parent = getParent();
        if (parent == nullptr) return -1;
        auto sp = sk_ref_sp<Actor>(this);
        return parent->children.indexOf(sp);
    }

    float Actor::getAlpha() const {
        return alpha;
    }
    void Actor::setAlpha(float alpha) {
        if(Actor::alpha != alpha){
            _actorListenerM.preFire();
            Actor::alpha = alpha;
            dispatchPropertyChanged(ActorListenerManager::TYPE_ALPHA);
        }
    }

    float Actor::getScrollX() const {
        return _scrollInfo != nullptr ? _scrollInfo->scrollX : 0;
    }
    void Actor::setScrollX(float scrollX) {
        if(_scrollInfo == nullptr){
            _scrollInfo = new ScrollInfo();
        }
        scrollTo(scrollX, _scrollInfo->scrollY);
    }
    float Actor::getScrollY() const {
        return _scrollInfo != nullptr ? _scrollInfo->scrollY : 0;
    }
    void Actor::setScrollY(float scrollY) {
        if(_scrollInfo == nullptr){
            _scrollInfo = new ScrollInfo();
        }
        scrollTo(_scrollInfo->scrollX, scrollY);
    }
    void Actor::scrollTo(float x, float y) {
        if(_scrollInfo == nullptr){
            _scrollInfo = new ScrollInfo();
        }
        if(_scrollInfo->scrollX != x || _scrollInfo->scrollY != y){
            float dx = x - _scrollInfo->scrollX;
            float dy = y - _scrollInfo->scrollY;
            _scrollInfo->scrollX = x;
            _scrollInfo->scrollY = y;
            //fire scroll changed
            onScrollChanged(dx, dy);
            ScrollEvent event;
            event.setScrollState(ScrollInfo::SCROLL_STATE_NONE);
            event.setDx(dx);
            event.setDy(dy);
            fire(event);
        }
    }
    void Actor::scrollBy(float dx, float dy) {
        if(_scrollInfo == nullptr){
            _scrollInfo = new ScrollInfo();
        }
        if(dx != 0 || dy != 0){
            scrollTo(_scrollInfo->scrollX + dx, _scrollInfo->scrollY + dy);
        }
    }
    void Actor::setScrollState(unsigned char newState) {
        if(_scrollInfo == nullptr){
            _scrollInfo = new ScrollInfo();
        }
        if(_scrollInfo->scrollState != newState){
            _scrollInfo->scrollState = newState;
            //fire state changed
            ScrollEvent event;
            event.setScrollState(newState);
            event.setDx(0);
            event.setDy(0);
            fire(event);
        }
    }
    void Actor::stopScroll() {
        setScrollState(ScrollInfo::SCROLL_STATE_IDLE);
    }
}