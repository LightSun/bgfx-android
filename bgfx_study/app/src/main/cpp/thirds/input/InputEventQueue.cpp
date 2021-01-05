//
// Created by Administrator on 2020/11/27 0027.
//

#include <log.h>
#include "InputEventQueue.h"
#include "lua/SkFloatBits.h"
#include "GestureContext.h"
#include "global.h"

namespace h7 {

    void InputEventQueue::drain() {
        lock();
        if (processor == NULL) {
            queue.clear();
            return;
        }
        processingQueue.addAll(&queue);
        queue.clear();
        unlock();

       // int[] q = processingQueue.items;
        IntArray& q = processingQueue;
        InputProcessor& localProcessor = *processor;
        for (int i = 0, n = processingQueue.size(); i < n;) {
           // int type = q[i++];
            int type = processingQueue.get(i++);
            currentEventTime = (long)q[i++] << 32 | q[i++] & 0xFFFFFFFFL;
            switch (type) {
                case SKIP:
                    i += processingQueue.get(i);
                    break;
                case KEY_DOWN:
                    localProcessor.keyDown(q[i++]);
                    break;
                case KEY_UP:
                    localProcessor.keyUp(q[i++]);
                    break;
                case KEY_TYPED:
                    localProcessor.keyTyped((char)q[i++]);
                    break;
                case TOUCH_DOWN:
                    localProcessor.touchDown(q[i++], q[i++], q[i++], q[i++]);
                    break;
                case TOUCH_UP:
                    localProcessor.touchUp(q[i++], q[i++], q[i++], q[i++]);
                    break;
                case TOUCH_DRAGGED:
                    localProcessor.touchDragged(q[i++], q[i++], q[i++]);
                    break;
                case MOUSE_MOVED:
                    localProcessor.mouseMoved(q[i++], q[i++]);
                    break;
                case SCROLLED:
                    localProcessor.scrolled(SkBits2Float(q[i++]) , SkBits2Float(q[i++]));
                    break;
                default:
                    LOGE("unsupport event type: %d", type);
                    //throw new RuntimeException();
            }
        }
        processingQueue.clear();
    }

    int InputEventQueue::next(int nextType, int i) {
        IntArray& q = queue;
        for (int n = queue.size(); i < n;) {
            int type = q[i];
            if (type == nextType) return i;
            i += 3;
            switch (type) {
                case SKIP:
                    i += q[i];
                    break;
                case KEY_DOWN:
                    i++;
                    break;
                case KEY_UP:
                    i++;
                    break;
                case KEY_TYPED:
                    i++;
                    break;
                case TOUCH_DOWN:
                    i += 4;
                    break;
                case TOUCH_UP:
                    i += 4;
                    break;
                case TOUCH_DRAGGED:
                    i += 3;
                    break;
                case MOUSE_MOVED:
                    i += 2;
                    break;
                case SCROLLED:
                    i += 2;
                    break;
                default:
                    LOGE("unsupport event type: %d", type);
            }
        }
        return -1;
    }

    void InputEventQueue::queueTime() {
        unsigned long long time = getCurrentEventTime();
        int t = (int)(time >> 32);
        queue.add(t);
        t = static_cast<int>(time);
        queue.add(t);
    }
    bool InputEventQueue::keyDown(int keycode) {
        lock();
        queue.add(const_cast<int &>(KEY_DOWN));
        queueTime();
        queue.add(keycode);
        unlock();
        return false;
    }
    bool InputEventQueue::keyUp(int keycode) {
        lock();
        queue.add(crCast_ref(int, KEY_UP));
        queueTime();
        queue.add(keycode);
        unlock();
        return false;
    }
    bool InputEventQueue::keyTyped(char ch) {
        lock();
        queue.add(crCast_ref(int,KEY_TYPED));
        queueTime();
        queue.add(rCast_ref(int, ch));
        unlock();
        return false;
    }
    bool InputEventQueue::touchDown (int screenX, int screenY, int pointer, int button) {
        lock();
        queue.add(crCast_ref(int, TOUCH_DOWN));
        queueTime();
        queue.add(screenX);
        queue.add(screenY);
        queue.add(pointer);
        queue.add(button);
        unlock();
        return false;
    }

    bool InputEventQueue::touchUp (int screenX, int screenY, int pointer, int button) {
        lock();
        queue.add(crCast_ref(int, TOUCH_UP));
        queueTime();
        queue.add(screenX);
        queue.add(screenY);
        queue.add(pointer);
        queue.add(button);
        unlock();
        return false;
    }

    bool InputEventQueue::touchDragged (int screenX, int screenY, int pointer) {
        lock();
        // Skip any queued touch dragged events for the same pointer.
        int val = 3;
        for (int i = next(TOUCH_DRAGGED, 0); i >= 0; i = next(TOUCH_DRAGGED, i + 6)) {
            if (queue.get(i + 5) == pointer) {
                queue.set(i, crCast_ref(int, SKIP));
                queue.set(i + 3, val);
            }
        }
        queue.add(crCast_ref(int, TOUCH_DRAGGED));
        queueTime();
        queue.add(screenX);
        queue.add(screenY);
        queue.add(pointer);
        unlock();
        return false;
    }

    bool InputEventQueue::mouseMoved (int screenX, int screenY) {
        int val = 2;
        lock();
        // Skip any queued mouse moved events.
        for (int i = next(MOUSE_MOVED, 0); i >= 0; i = next(MOUSE_MOVED, i + 5)) {
            queue.set(i, crCast_ref(int, SKIP));
            queue.set(i + 3, val);
        }
        queue.add(crCast_ref(int, MOUSE_MOVED));
        queueTime();
        queue.add(screenX);
        queue.add(screenY);
        unlock();
        return false;
    }
    bool InputEventQueue::scrolled(float amountX, float amountY) {
        queue.add(crCast_ref(int, SCROLLED));
        queueTime();
        int32_t val = SkFloat2Bits(amountX);
        queue.add(val);
        val = SkFloat2Bits(amountY);
        queue.add(val);
        return false;
    }
}