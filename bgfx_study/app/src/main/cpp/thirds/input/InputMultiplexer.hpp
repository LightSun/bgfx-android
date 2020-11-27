//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_INPUTMULTIPLEXER_HPP
#define BGFX_STUDY_INPUTMULTIPLEXER_HPP

#include "InputProcessor.h"
#include "../utils/Array.hpp"

namespace h7 {
    class InputMultiplexer : public InputProcessor {

    public:
        Array<InputProcessor*> processors;

        InputMultiplexer() {}

        inline InputMultiplexer(Array<InputProcessor *>* array) {
            processors.addAll(array);
        }

        inline void addProcessors(int index, Array<InputProcessor *> *array) {
            processors.addAll(index, array);
        }

        inline void addProcessor(int index, InputProcessor *processor) {
            processors.add(index, processor);
        }

        inline void addProcessor(InputProcessor *processor) {
            processors.add(processor);
        }

        inline void removeProcessor(int index) {
            processors.removeAt(index);
        }

        inline void removeProcessor(InputProcessor *processor) {
            processors.remove(processor);
        }

        inline int size() {
            return processors.size();
        }
        inline void clear() {
            auto func = [&](Array<InputProcessor *>* arr, int index, InputProcessor *& ele){
                ele->unRefAndDestroy();
                return false;
            };
            processors.travel(reinterpret_cast<std::function<bool(Array<InputProcessor*>*, int, InputProcessor*&)> &>(func));
            processors.clear();
        }
        inline void setProcessors(Array<InputProcessor*>* processors) {
            clear();
            this->processors = *processors;
        }
        inline Array<InputProcessor*>& getProcessors() {
            return processors;
        }
        inline bool keyDown(int keycode) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->keyDown(keycode)){
                    return true;
                }
            }
            return false;
        }
        inline bool keyUp(int keycode) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->keyUp(keycode)){
                    return true;
                }
            }
            return false;
        }
        inline bool keyTyped(char ch) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->keyTyped(ch)){
                    return true;
                }
            }
            return false;
        }
        inline bool touchDown(int screenX, int screenY, int pointer, int button) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->touchDown(screenX, screenY, pointer, button)){
                    return true;
                }
            }
            return false;
        }
        inline bool touchUp(int screenX, int screenY, int pointer, int button) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->touchUp(screenX, screenY, pointer, button)){
                    return true;
                }
            }
            return false;
        }
        inline bool touchDragged (int screenX, int screenY, int pointer) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->touchDragged(screenX, screenY, pointer)){
                    return true;
                }
            }
            return false;
        }
        inline bool mouseMoved (int screenX, int screenY) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->mouseMoved(screenX, screenY)){
                    return true;
                }
            }
            return false;
        }
        inline bool scrolled(float amountX, float amountY) {
            Array<InputProcessor*> items = processors.copy();
            for (int i = 0, n = items.size(); i < n; i++){
                if ((items)[i]->scrolled(amountX, amountY)){
                    return true;
                }
            }
            return false;
        }
    };
}

#endif //BGFX_STUDY_INPUTMULTIPLEXER_HPP
