//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_LAYOUT_H
#define BGFX_STUDY_LAYOUT_H

namespace h7 {

    class Layout {
    public:
/** Computes and caches any information needed for drawing and, if this actor has children, positions and sizes each child,
	 * calls {@link #invalidate()} on any each child whose width or height has changed, and calls {@link #validate()} on each
	 * child. This method should almost never be called directly, instead {@link #validate()} should be used. */
        virtual void layout() {};

        /** Invalidates this actor's layout, causing {@link #layout()} to happen the next time {@link #validate()} is called. This
         * method should be called when state changes in the actor that requires a layout but does not change the minimum, preferred,
         * maximum, or actual size of the actor (meaning it does not affect the parent actor's layout). */
        virtual void invalidate() {};

        /** Invalidates this actor and its ascendants, calling {@link #invalidate()} on each. This method should be called when state
         * changes in the actor that affects the minimum, preferred, maximum, or actual size of the actor (meaning it potentially
         * affects the parent actor's layout). */
        virtual void invalidateHierarchy() {};

        /** Ensures the actor has been laid out. Calls {@link #layout()} if {@link #invalidate()} has been called since the last time
         * {@link #validate()} was called, or if the actor otherwise needs to be laid out. This method is usually called in
         * {@link Actor#draw(Batch, float)} by the actor itself before drawing is performed. */
        virtual void validate() {};

        /** Sizes this actor to its preferred width and height, then calls {@link #validate()}.
         * <p>
         * Generally this method should not be called in an actor's constructor because it calls {@link #layout()}, which means a
         * subclass would have layout() called before the subclass' constructor. Instead, in constructors simply set the actor's size
         * to {@link #getPrefWidth()} and {@link #getPrefHeight()}. This allows the actor to have a size at construction time for more
         * convenient use with groups that do not layout their children. */
        virtual void pack() {};

        /** If true, this actor will be sized to the parent in {@link #validate()}. If the parent is the stage, the actor will be sized
         * to the stage. This method is for convenience only when the widget's parent does not set the size of its children (such as
         * the stage). */
        virtual void setFillParent(bool fillParent) {
            this->fillParent = fillParent;
        };

        /** Enables or disables the layout for this actor and all child actors, recursively. When false, {@link #validate()} will not
         * cause a layout to occur. This can be useful when an actor will be manipulated externally, such as with actions. Default is
         * true. */
        virtual void setLayoutEnabled(bool enabled) {
            this->layoutEnabled = enabled;
        }

        /** Returns true if the widget's layout has been {@link #invalidate() invalidated}. */
        bool markNeedsLayout() {
            return _needsLayout;
        }
        virtual float getMinWidth() { return getPrefWidth(); }

        virtual float getMinHeight() { return getPrefHeight(); }

        virtual float getPrefWidth() { return 0; }

        virtual float getPrefHeight() { return 0; }

        /** Zero indicates no max width. */
        virtual float getMaxWidth() { return 0; }

        /** Zero indicates no max height. */
        virtual float getMaxHeight() { return 0; }

    protected:
        bool _needsLayout = true;
        bool fillParent;
        bool layoutEnabled = true;
    };
}

#endif //BGFX_STUDY_LAYOUT_H
