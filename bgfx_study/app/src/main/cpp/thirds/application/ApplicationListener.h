//
// Created by Administrator on 2020/12/7 0007.
//

#ifndef BGFX_STUDY_APPLICATIONLISTENER_H
#define BGFX_STUDY_APPLICATIONLISTENER_H

#include <lua/SkRefCnt.h>

namespace h7 {
    class ApplicationListener: public SkRefCnt {
    public:
        /** Called when the {@link Application} is first created. */
        virtual void create() {

        }
        /** Called when the {@link Application} is resized. This can happen at any point during a non-paused state but will never happen
         * before a call to {@link #create()}.
         *
         * @param width the new width in pixels
         * @param height the new height in pixels */
        virtual void resize(int width, int height){

        }
        /** Called when the {@link Application} should render itself. */
        virtual void render(){

        }
        /** Called when the {@link Application} is paused, usually when it's not active or visible on-screen. An Application is also
         * paused before it is destroyed. */
        virtual void pause(){

        }
        /** Called when the {@link Application} is resumed from a paused state, usually when it regains focus. */
        virtual void resume(){

        }
        /** Called when the {@link Application} is destroyed. Preceded by a call to {@link #pause()}. */
        virtual void dispose(){

        }
    };
}

#endif //BGFX_STUDY_APPLICATIONLISTENER_H
