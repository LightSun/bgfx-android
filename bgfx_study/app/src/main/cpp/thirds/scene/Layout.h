//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_LAYOUT_H
#define BGFX_STUDY_LAYOUT_H

namespace h7 {

    class Layout {
    public:
        static constexpr uint8_t LAYOUT_TYPE_MATCH_PARENT = 1;
        static constexpr uint8_t LAYOUT_TYPE_WRAP_CONTENT = 2;
        static constexpr uint8_t LAYOUT_TYPE_REAL = 3;

        virtual void setLayoutType(uint8_t type){
            this->_layoutType = type;
        }
        inline uint8_t getLayoutType(){
            return _layoutType;
        }
        virtual void setLayoutEnabled(bool enabled) {
            this->layoutEnabled = enabled;
        }
        inline bool isLayoutEnabled(){
            return layoutEnabled;
        }

        virtual float getMinWidth() { return 0; }
        virtual float getMinHeight() { return 0; }

        virtual float getMaxWidth() { return 0; }
        virtual float getMaxHeight() { return 0; }

        /**
         * call this to measure the content width and height.
         * you should call setSize(...) in it.
         */
        virtual void measure(){}

    private:
        uint8_t _layoutType;
        bool layoutEnabled = true;
    };
}

#endif //BGFX_STUDY_LAYOUT_H
