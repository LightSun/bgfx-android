//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_BATCHINGLISTUPDATECALLBACK_H
#define BGFX_STUDY_BATCHINGLISTUPDATECALLBACK_H

#include "ListUpdateCallback.h"
#include "../lua/SkRefCnt.h"
#include "../../include/bx/bx.h"

namespace h7 {
/**
 * Wraps a {@link ListUpdateCallback} callback and batches operations that can be merged.
 * <p>
 * For instance, when 2 add operations comes that adds 2 consecutive elements,
 * BatchingListUpdateCallback merges them and calls the wrapped callback only once.
 * <p>
 * This is a general purpose class and is also used by
 * {@link DiffUtil.DiffResult DiffResult} and
 * {@link SortedList} to minimize the number of updates that are dispatched.
 * <p>
 * If you use this class to batch updates, you must call {@link #dispatchLastEvent()} when the
 * stream of update events drain.
 */
    class BatchingListUpdateCallback : public ListUpdateCallback {

    private:
        static constexpr int TYPE_NONE = 0;
        static constexpr int TYPE_ADD = 1;
        static constexpr int TYPE_REMOVE = 2;
        static constexpr int TYPE_CHANGE = 3;

        sk_sp<ListUpdateCallback> mWrapped;

        int mLastEventType = TYPE_NONE;
        int mLastEventPosition = -1;
        int mLastEventCount = -1;
        void* mLastEventPayload = nullptr;

    public:

        BatchingListUpdateCallback(sk_sp<ListUpdateCallback> sp) {
            mWrapped = sp;
        }

        /**
         * BatchingListUpdateCallback holds onto the last event to see if it can be merged with the
         * next one. When stream of events finish, you should call this method to dispatch the last
         * event.
         */
        void dispatchLastEvent() {
            if (mLastEventType == TYPE_NONE) {
                return;
            }
            switch (mLastEventType) {
                case TYPE_ADD:
                    mWrapped->onInserted(mLastEventPosition, mLastEventCount);
                    break;
                case TYPE_REMOVE:
                    mWrapped->onRemoved(mLastEventPosition, mLastEventCount);
                    break;
                case TYPE_CHANGE:
                    mWrapped->onChanged(mLastEventPosition, mLastEventCount, mLastEventPayload);
                    break;
            }
            mLastEventPayload = nullptr;
            mLastEventType = TYPE_NONE;
        }

        void onInserted(int position, int count) {
            if (mLastEventType == TYPE_ADD && position >= mLastEventPosition
                && position <= mLastEventPosition + mLastEventCount) {
                mLastEventCount += count;
                mLastEventPosition = bx::min(position, mLastEventPosition);
                return;
            }
            dispatchLastEvent();
            mLastEventPosition = position;
            mLastEventCount = count;
            mLastEventType = TYPE_ADD;
        }
        void onRemoved(int position, int count) {
            if (mLastEventType == TYPE_REMOVE && mLastEventPosition >= position &&
                mLastEventPosition <= position + count) {
                mLastEventCount += count;
                mLastEventPosition = position;
                return;
            }
            dispatchLastEvent();
            mLastEventPosition = position;
            mLastEventCount = count;
            mLastEventType = TYPE_REMOVE;
        }

        void onMoved(int fromPosition, int toPosition) {
            dispatchLastEvent(); // moves are not merged
            mWrapped->onMoved(fromPosition, toPosition);
        }

        void onChanged(int position, int count, void* payload) {
            if (mLastEventType == TYPE_CHANGE &&
                !(position > mLastEventPosition + mLastEventCount
                  || position + count < mLastEventPosition || mLastEventPayload != payload)) {
                // take potential overlap into account
                int previousEnd = mLastEventPosition + mLastEventCount;
                mLastEventPosition = bx::min(position, mLastEventPosition);
                mLastEventCount = bx::max(previousEnd, position + count) - mLastEventPosition;
                return;
            }
            dispatchLastEvent();
            mLastEventPosition = position;
            mLastEventCount = count;
            mLastEventPayload = payload;
            mLastEventType = TYPE_CHANGE;
        }
    };
}

#endif //BGFX_STUDY_BATCHINGLISTUPDATECALLBACK_H
