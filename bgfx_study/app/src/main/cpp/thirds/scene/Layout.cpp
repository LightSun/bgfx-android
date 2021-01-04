//
// Created by Administrator on 2020/12/25 0025.
//
#include "Layout.h"
#include "SceneHelper.h"

namespace h7 {
    Layout::Layout(): minInfo(nullptr), maxInfo(nullptr) {
        lp.reset(new LayoutParams());
        setNeedMeasure(true);
        setNeedLayout(true);
    }
    Layout::~Layout() {
        DESTROY_POINTER(minInfo)
        DESTROY_POINTER(maxInfo)
    }
    void Layout::setNeedLayout(bool need) {
        if(need){
            rcCast(Actor*, this)->addFlags(Actor::FLAG_NEED_LAYOUT);
        } else{
            rcCast(Actor*, this)->deleteFlags(Actor::FLAG_NEED_LAYOUT);
        }
    }
    void Layout::setNeedMeasure(bool need) {
        if(need){
            rcCast(Actor*, this)->addFlags(Actor::FLAG_NEED_MEASURE);
        } else{
            rcCast(Actor*, this)->deleteFlags(Actor::FLAG_NEED_MEASURE);
        }
    }
    bool Layout::isNeedLayout(){
        return rcCast(Actor*, this)->hasFlags(Actor::FLAG_NEED_LAYOUT);
    }
    bool Layout::isNeedMeasure(){
        return rcCast(Actor*, this)->hasFlags(Actor::FLAG_NEED_MEASURE);
    }
    void Layout::layoutSize(float w1, float h1, float ew, float eh) {
        auto pActor = rCast(Actor*, this);
        float w = w1;
        float h = h1;
        bool hasWrapContent = false;
        switch (getLayoutWidthType()) {
            case LayoutParams::LAYOUT_MATCH_PARENT:
                w = ew - pActor->getMarginHorizontal();
                break;
            case LayoutParams::LAYOUT_WRAP_CONTENT:
                hasWrapContent = true;
                break;
            case LayoutParams::LAYOUT_REAL:
                float minW = getMinWidth();
                if (minW > 0 && w < minW) {
                    w = minW;
                }
                float maxW = getMaxHeight();
                if(maxW > 0 && w > maxW){
                    w = maxW;
                }
                break;
        }
        switch (getLayoutHeightType()) {
            case LayoutParams::LAYOUT_MATCH_PARENT:
                h = eh - pActor->getMarginVertical();
                break;
            case LayoutParams::LAYOUT_WRAP_CONTENT:
                hasWrapContent = true;
                break;
            case LayoutParams::LAYOUT_REAL:
                float minH = getMinHeight();
                if (minH > 0 && h < minH) {
                    h = minH;
                }
                float maxH = getMaxHeight();
                if(maxH > 0 && h > maxH){
                    h = maxH;
                }
                break;
        }
        //need measure
        if (hasWrapContent) {
            doMeasure(ew - pActor->getMarginHorizontal(), eh - pActor->getMarginVertical(), w, h);
        }
        pActor->setSize(w, h);
    }

    signed char Layout::getLayoutWidthType() const {
        return lp->layoutWidthType;
    }

    void Layout::setLayoutWidthType(signed char layoutWidthType) {
        lp->layoutWidthType = layoutWidthType;
    }

    signed char Layout::getLayoutHeightType() const {
        return lp->layoutHeightType;
    }

    void Layout::setLayoutHeightType(signed char layoutHeightType) {
        lp->layoutHeightType = layoutHeightType;
    }

    LayoutParams *Layout::getLayoutParams() const {
        return lp.get();
    }

    void Layout::setLayoutParams(h7::LayoutParams *in) {
        if(in != nullptr){
            lp.reset(in);
        }
    }

    float Layout::getMinWidth() const { return minInfo != nullptr ? minInfo->w : 0; }

    float Layout::getMinHeight() const { return minInfo != nullptr ? minInfo->h : 0; }

    float Layout::getMaxWidth() const { return maxInfo != nullptr ? maxInfo->w : 0;}

    float Layout::getMaxHeight() const { return maxInfo != nullptr ? maxInfo->h : 0; }

    void Layout::setMinWidth(float w) {
        if(minInfo == nullptr){
            minInfo = new WH();
        }
        minInfo->w = w;
    }
    void Layout::setMinHeight(float h) {
        if(minInfo == nullptr){
            minInfo = new WH();
        }
        minInfo->h = h;
    }

    void Layout::setMaxWidth(float w) {
        if(maxInfo == nullptr){
            maxInfo = new WH();
        }
        maxInfo->w = w;
    }

    void Layout::setMaxHeight(float h) {
        if(maxInfo == nullptr){
            maxInfo = new WH();
        }
        maxInfo->h = h;
    }
    void Layout::doMeasure(float restrictW, float restrictH, float &outW, float &outH) {
        if(isNeedMeasure()){
            setNeedMeasure(false);
            float w, h;
            measure(restrictW, restrictH, w, h);
            auto padding = rCast(Actor*, this)->getPadding();
            if(padding != nullptr){
                w += padding->horizontal();
                h += padding->vertical();
            }
            outW = w;
            outH = h;
        }
    }
    void Layout::measure(float restrictW, float restrictH, float &outW, float &outH) {
        BX_UNUSED(restrictW, restrictH)
        outW = getExpectWidth();
        outH = getExpectHeight();
    }

    float Layout::getExpectWidth() {
        float w = rCast(Actor*, this)->getWidth();
        float minW = getMinWidth();
        if (minW > 0 && w < minW) {
            w = minW;
        }
        float maxW = getMaxHeight();
        if(maxW > 0 && w > maxW){
            w = maxW;
        }
        return w;
    }
    float Layout::getExpectHeight() {
        float h = rCast(Actor*, this)->getHeight();
        float minH = getMinHeight();
        if (minH > 0 && h < minH) {
            h = minH;
        }
        float maxH = getMaxHeight();
        if(maxH > 0 && h > maxH){
            h = maxH;
        }
        return h;
    }
}