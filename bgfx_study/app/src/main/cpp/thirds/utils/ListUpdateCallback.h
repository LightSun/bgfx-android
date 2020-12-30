//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_LISTUPDATECALLBACK_H
#define BGFX_STUDY_LISTUPDATECALLBACK_H

#include "../lua/SkRefCnt.h"

namespace h7 {

/**
 * An interface that can receive Update operations that are applied to a list.
 * <p>
 * This class can be used together with DiffUtil to detect changes between two lists.
 */
    class ListUpdateCallback: public SkRefCnt {
    public:
        /**
         * Called when {@code count} number of items are inserted at the given position.
         *
         * @param position The position of the new item.
         * @param count    The number of items that have been added.
         */
        virtual void onInserted(int position, int count) {
            BX_UNUSED(position, count)
        }

        /**
         * Called when {@code count} number of items are removed from the given position.
         *
         * @param position The position of the item which has been removed.
         * @param count    The number of items which have been removed.
         */
        virtual void onRemoved(int position, int count) {
            BX_UNUSED(position, count)
        }

        /**
         * Called when an item changes its position in the list.
         *
         * @param fromPosition The previous position of the item before the move.
         * @param toPosition   The new position of the item.
         */
        virtual void onMoved(int fromPosition, int toPosition) {
            BX_UNUSED(fromPosition, toPosition)
        }

        /**
         * Called when {@code count} number of items are updated at the given position.
         *
         * @param position The position of the item which has been updated.
         * @param count    The number of items which has changed.
         */
        virtual void onChanged(int position, int count, void *payload) {
            BX_UNUSED(position, count, payload)
        }
    };
}

#endif //BGFX_STUDY_LISTUPDATECALLBACK_H
