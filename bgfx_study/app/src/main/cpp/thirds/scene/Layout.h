//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_LAYOUT_H
#define BGFX_STUDY_LAYOUT_H

#include <lua/SkRefCnt.h>

#define LP_TYPE 0
#define LP_TYPE_LINEAR 1

namespace h7 {
    class Actor;

    typedef struct LayoutParams {
        static constexpr signed char LAYOUT_MATCH_PARENT = -1;
        static constexpr signed char LAYOUT_WRAP_CONTENT = -2;
        static constexpr signed char LAYOUT_REAL = -3;

        signed char layoutWidthType = LAYOUT_REAL;
        signed char layoutHeightType = LAYOUT_REAL;
        unsigned char lpType = LP_TYPE;
    } LayoutParams;

    class Layout {
    private:
        typedef struct WH{
            float w = 0;
            float h = 0;
        }WH;
    public:
        Layout();
        virtual ~Layout();
        virtual void setLayoutEnabled(bool enabled);

        inline bool isLayoutEnabled() const;

        signed char getLayoutWidthType() const;

        void setLayoutWidthType(signed char layoutWidthType);

        signed char getLayoutHeightType() const;

        void setLayoutHeightType(signed char layoutHeightType);

        LayoutParams* getLayoutParams() const;

        void setLayoutParams(LayoutParams *in);

        float getMinWidth() const;

        float getMinHeight() const;

        float getMaxWidth() const;

        float getMaxHeight() const;

        void setMinWidth(float minWidth);

        void setMinHeight(float minHeight);

        void setMaxWidth(float maxWidth);

        void setMaxHeight(float maxHeight);

        /**
         * call this to measure the content width and height.
         * @param outW the out width
         * @param outH the out height
         */
        virtual void measure(float &outW, float &outH) {
        }
    protected:
        /**
        * call this to compute layout and set size.
        * @param w the current width of layout
        * @param h the current height of layout
        * @param ew the expect width which is from parent
        * @param eh the expect height which is from parent
        */
        void layoutSize(float w, float h, float ew, float eh);

    private:
        bool layoutEnabled = true;
        WH* minInfo;
        WH* maxInfo;
        sk_sp<LayoutParams> lp;
    };
}

#endif //BGFX_STUDY_LAYOUT_H
