//
// Created by Administrator on 2020/12/28 0028.
//

#include <stdio.h>
#include <stdarg.h>

#include <log.h>
#include "SortedList.h"
#include "Arrays.h"

namespace h7{

    class SpItemDelegate_Com: public Comparator<sk_sp<SortedList::ItemDelegate>>{
    public:
        SpItemDelegate_Com(const SortedList::Callback &cb) : cb(cb) {}
        virtual int compare(const sk_sp<SortedList::ItemDelegate>* o1, const sk_sp<SortedList::ItemDelegate>* o2){
            return cb.compare(o1->get(), o2->get());
        }
    private:
        SortedList::Callback cb;
    };
    //--------------------- privates -------------------

    void SortedList::throwIfInMutationOperation(){
        if (!mOldData.isEmpty()) {
            LOGE("Data cannot be mutated in the middle of a batch update operation such as addAll or replaceAll.");
        }
    }
    int SortedList::add(h7::SortedList::ItemDelegate *item, bool notify) {
        int index = findIndexOf(item, mData, 0, mSize, INSERTION);
        if (index == INVALID_POSITION) {
            index = 0;
        } else if (index < mSize) {
            sk_sp<ItemDelegate> existing = mData[index];
            if (mCallback->areItemsTheSame(existing.get(), item)) {
                if (mCallback->areContentsTheSame(existing.get(), item)) {
                    //no change but still replace the item
                    mData[index] = item;
                    return index;
                } else {
                    mData[index] = item;
                    mCallback->onChanged(index, 1, mCallback->getChangePayload(existing.get(), item));
                    return index;
                }
            }
        }
        addToData(index, item);
        if (notify) {
            mCallback->onInserted(index, 1);
        }
        return index;
    }

    void SortedList::removeItemAtIndex(int index, bool notify) {
        mData.setRegion(index, mData, index + 1, mSize - index - 1);
        //System.arraycopy(mData, index + 1, mData, index, mSize - index - 1);
        mSize--;
        mData[mSize] = nullptr;
        if (notify) {
            mCallback->onRemoved(index, 1);
        }
    }
    bool SortedList::remove(h7::SortedList::ItemDelegate *item, bool notify) {
        int index = findIndexOf(item, mData, 0, mSize, DELETION);
        if (index == INVALID_POSITION) {
            return false;
        }
        removeItemAtIndex(index, notify);
        return true;
    }
    int SortedList::findIndexOf(h7::SortedList::ItemDelegate *item,
                                h7::Array<sk_sp<ItemDelegate>> &mData, int left, int right,
                                int reason) {
        while (left < right) {
            const int middle = (left + right) / 2;
            sk_sp<ItemDelegate> myItem = mData[middle];
            const int cmp = mCallback->compare(myItem.get(), item);
            if (cmp < 0) {
                left = middle + 1;
            } else if (cmp == 0) {
                if (mCallback->areItemsTheSame(myItem.get(), item)) {
                    return middle;
                } else {
                    int exact = linearEqualitySearch(item, middle, left, right);
                    if (reason == INSERTION) {
                        return exact == INVALID_POSITION ? middle : exact;
                    } else {
                        return exact;
                    }
                }
            } else {
                right = middle;
            }
        }
        return reason == INSERTION ? left : INVALID_POSITION;
    }
    int SortedList::linearEqualitySearch(SortedList::ItemDelegate * item, int middle, int left, int right) {
        // go left
        for (int next = middle - 1; next >= left; next--) {
            sk_sp<SortedList::ItemDelegate> nextItem = mData[next];
            int cmp = mCallback->compare(nextItem.get(), item);
            if (cmp != 0) {
                break;
            }
            if (mCallback->areItemsTheSame(nextItem.get(), item)) {
                return next;
            }
        }
        for (int next = middle + 1; next < right; next++) {
            sk_sp<SortedList::ItemDelegate> nextItem = mData[next];
            int cmp = mCallback->compare(nextItem.get(), item);
            if (cmp != 0) {
                break;
            }
            if (mCallback->areItemsTheSame(nextItem.get(), item)) {
                return next;
            }
        }
        return INVALID_POSITION;
    }
    void SortedList::addToData(int index, h7::SortedList::ItemDelegate *item) {
        if (index > mSize) {
            LOGE( "cannot add item to %d,  because size is %d." , index, mSize);
        }
        if (mSize == mData.size()) {
            mData.setSize(mData.size() + CAPACITY_GROWTH);
        }
        auto sitem = sk_ref_sp(item);
        mData.add(index, sitem);
        mSize++;
    }
    void SortedList::replaceAllRemove() {
        mSize--;
        mOldDataStart++;
        mCallback->onRemoved(mNewDataStart, 1);
    }
    void SortedList::replaceAllInsert(h7::SortedList::ItemDelegate *newItem) {
        mData[mNewDataStart].reset(newItem);
        mNewDataStart++;
        mSize++;
        mCallback->onInserted(mNewDataStart - 1, 1);
    }
    void SortedList::replaceAllInternal(h7::Array<sk_sp<ItemDelegate>> &newData) {
        const bool forceBatchedUpdates = !(mCallback->isBatch());
        if (forceBatchedUpdates) {
            beginBatchedUpdates();
        }

        mOldDataStart = 0;
        mOldDataSize = mSize;
        mOldData = mData;

        mNewDataStart = 0;
        int newSize = sortAndDedup(newData);
        mData.setSize(newSize);

        while (mNewDataStart < newSize || mOldDataStart < mOldDataSize) {
            if (mOldDataStart >= mOldDataSize) {
                int insertIndex = mNewDataStart;
                int itemCount = newSize - mNewDataStart;
                mData.setRegion(insertIndex, newData, insertIndex, itemCount);
               // System.arraycopy(newData, insertIndex, mData, insertIndex, itemCount);
                mNewDataStart += itemCount;
                mSize += itemCount;
                mCallback->onInserted(insertIndex, itemCount);
                break;
            }
            if (mNewDataStart >= newSize) {
                int itemCount = mOldDataSize - mOldDataStart;
                mSize -= itemCount;
                mCallback->onRemoved(mNewDataStart, itemCount);
                break;
            }

            sk_sp<SortedList::ItemDelegate> oldItem = mOldData[mOldDataStart];
            sk_sp<SortedList::ItemDelegate> newItem = newData[mNewDataStart];

            int result = mCallback->compare(oldItem.get(), newItem.get());
            if (result < 0) {
                replaceAllRemove();
            } else if (result > 0) {
                replaceAllInsert(newItem.get());
            } else {
                if (!mCallback->areItemsTheSame(oldItem.get(), newItem.get())) {
                    // The items aren't the same even though they were supposed to occupy the same
                    // place, so both notify to remove and add an item in the current location.
                    replaceAllRemove();
                    replaceAllInsert(newItem.get());
                } else {
                    mData[mNewDataStart] = newItem;
                    mOldDataStart++;
                    mNewDataStart++;
                    if (!mCallback->areContentsTheSame(oldItem.get(), newItem.get())) {
                        // The item is the same but the contents have changed, so notify that an
                        // onChanged event has occurred.
                        mCallback->onChanged(mNewDataStart - 1, 1,
                                            mCallback->getChangePayload(oldItem.get(), newItem.get()));
                    }
                }
            }
        }

        mOldData.clear();

        if (forceBatchedUpdates) {
            endBatchedUpdates();
        }
    }

    //-------------------- public ---------
    int SortedList::size() {
        return mSize;
    }
    void SortedList::beginBatchedUpdates() {
        throwIfInMutationOperation();
        if (mCallback->isBatch()) {
            return;
        }
        if (mBatchedCallback == nullptr) {
            mBatchedCallback.reset(new BatchedCallback(mCallback));
        }
        mCallback = mBatchedCallback;
    }
    void SortedList::endBatchedUpdates() {
        throwIfInMutationOperation();
        if (mCallback->isBatch()) {
            rCast(BatchedCallback*, mCallback.get())->dispatchLastEvent();
           // ((BatchedCallback) mCallback).dispatchLastEvent();
        }
        if (mCallback == mBatchedCallback) {
            mCallback = mBatchedCallback->mWrappedCallback;
        }
    }
    void SortedList::replaceAll(h7::Array<sk_sp<ItemDelegate>> &items, bool mayModifyInput) {
        throwIfInMutationOperation();

        if (mayModifyInput) {
            replaceAllInternal(items);
        } else {
            h7::Array<sk_sp<ItemDelegate>> tmpItems;
            for (int i = 0; i < items.size(); ++i) {
                auto sp = sk_ref_sp(items[i]->copy());
                tmpItems.add(sp);
            }
            replaceAllInternal(tmpItems);
        }
    }
    void SortedList::replaceAll(h7::Array<sk_sp<ItemDelegate>> &items) {
        replaceAll(items, true);
    }
    void SortedList::addAll(h7::Array<sk_sp<ItemDelegate>> &items, bool mayModifyInput) {
        throwIfInMutationOperation();
        if (items.size() == 0) {
            return;
        }
        if (mayModifyInput) {
            addAllInternal(items);
        } else {
            for (int i = 0; i < items.size(); ++i) {
                add(items.get(i).get()->copy());
            }
        }
    }
    void SortedList::addAll(int n, ...) {
        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; ++i) {
            add(va_arg(args, ItemDelegate *));
        }
        va_end(args);
    }
    void SortedList::addAll(h7::Array<sk_sp<ItemDelegate>> &items) {
        addAll(items, true);
    }
    int SortedList::add(const h7::SortedList::ItemDelegate *item) {
        throwIfInMutationOperation();
        return add(const_cast<ItemDelegate *>(item), true);
    }

    int SortedList::sortAndDedup(h7::Array<sk_sp<ItemDelegate>> &items) {
        if (items.size() == 0) {
            return 0;
        }

        // Arrays.sort is stable.
        SpItemDelegate_Com com = SpItemDelegate_Com(*mCallback);
        Arrays::sort(items, rCast(Comparator<sk_sp<SortedList::ItemDelegate>>*, &com));

        // Keep track of the range of equal items at the end of the output.
        // Start with the range containing just the first item.
        int rangeStart = 0;
        int rangeEnd = 1;

        for (int i = 1; i < items.size(); ++i) {
            sk_sp<ItemDelegate> currentItem = items[i];

            int compare = mCallback->compare(items[rangeStart].get(), currentItem.get());

            if (compare == 0) {
                // The range of equal items continues, update it.
                const int sameItemPos = findSameItem(currentItem.get(), items, rangeStart, rangeEnd);
                if (sameItemPos != INVALID_POSITION) {
                    // Replace the duplicate item.
                    items[sameItemPos] = currentItem;
                } else {
                    // Expand the range.
                    if (rangeEnd != i) {  // Avoid redundant copy.
                        items[rangeEnd] = currentItem;
                    }
                    rangeEnd++;
                }
            } else {
                // The range has ended. Reset it to contain just the current item.
                if (rangeEnd != i) {  // Avoid redundant copy.
                    items[rangeEnd] = currentItem;
                }
                rangeStart = rangeEnd++;
            }
        }
        return rangeEnd;
    }
    int SortedList::findSameItem(h7::SortedList::ItemDelegate* item,
                                 h7::Array<sk_sp<ItemDelegate>> &items, int from, int to) {
        for (int pos = from; pos < to; pos++) {
            if (mCallback->areItemsTheSame(items[pos].get(), item)) {
                return pos;
            }
        }
        return INVALID_POSITION;
    }
    void SortedList::addAllInternal(h7::Array<sk_sp<ItemDelegate>> &newItems) {
        if (newItems.size() < 1) {
            return;
        }

        int newSize = sortAndDedup(newItems);

        if (mSize == 0) {
            mData = newItems;
            mSize = newSize;
            mCallback->onInserted(0, newSize);
        } else {
            merge(newItems, newSize);
        }
    }
    void SortedList::merge(h7::Array<sk_sp<ItemDelegate>> &newData, int newDataSize) {
        const bool forceBatchedUpdates = !(mCallback->isBatch());
        if (forceBatchedUpdates) {
            beginBatchedUpdates();
        }

        mOldData = mData;
        mOldDataStart = 0;
        mOldDataSize = mSize;

        const int mergedCapacity = mSize + newDataSize + CAPACITY_GROWTH;
        mData.setCapacity(mergedCapacity);
        // mData = (T[]) Array.newInstance(mTClass, mergedCapacity);
        mNewDataStart = 0;

        int newDataStart = 0;
        while (mOldDataStart < mOldDataSize || newDataStart < newDataSize) {
            if (mOldDataStart == mOldDataSize) {
                // No more old items, copy the remaining new items.
                int itemCount = newDataSize - newDataStart;
                mData.setRegion(mNewDataStart, newData, newDataStart, itemCount);
                //System.arraycopy(newData, newDataStart, mData, mNewDataStart, itemCount);
                mNewDataStart += itemCount;
                mSize += itemCount;
                mCallback->onInserted(mNewDataStart - itemCount, itemCount);
                break;
            }

            if (newDataStart == newDataSize) {
                // No more new items, copy the remaining old items.
                int itemCount = mOldDataSize - mOldDataStart;
                mData.setRegion(mNewDataStart, mOldData, mOldDataStart, itemCount);
                //System.arraycopy(mOldData, mOldDataStart, mData, mNewDataStart, itemCount);
                mNewDataStart += itemCount;
                break;
            }

            sk_sp<ItemDelegate> oldItem = mOldData[mOldDataStart];
            sk_sp<ItemDelegate> newItem = newData[newDataStart];
            int compare = mCallback->compare(oldItem.get(), newItem.get());
            if (compare > 0) {
                // New item is lower, output it.
                mData[mNewDataStart++] = newItem;
                mSize++;
                newDataStart++;
                mCallback->onInserted(mNewDataStart - 1, 1);
            } else if (compare == 0 && mCallback->areItemsTheSame(oldItem.get(), newItem.get())) {
                // Items are the same. Output the new item, but consume both.
                mData[mNewDataStart++] = newItem;
                newDataStart++;
                mOldDataStart++;
                if (!mCallback->areContentsTheSame(oldItem.get(), newItem.get())) {
                    mCallback->onChanged(mNewDataStart - 1, 1,
                                        mCallback->getChangePayload(oldItem.get(), newItem.get()));
                }
            } else {
                // Old item is lower than or equal to (but not the same as the new). Output it.
                // New item with the same sort order will be inserted later.
                mData[mNewDataStart++] = oldItem;
                mOldDataStart++;
            }
        }

        mOldData.clear();

        if (forceBatchedUpdates) {
            endBatchedUpdates();
        }
    }
    bool SortedList::remove(const h7::SortedList::ItemDelegate *item){
        throwIfInMutationOperation();
        return remove(const_cast<ItemDelegate *>(item), true);
    }
    const SortedList::ItemDelegate* SortedList::removeItemAt(int index) {
        throwIfInMutationOperation();
        const SortedList::ItemDelegate* item = get(index);
        removeItemAtIndex(index, true);
        return item;
    }
    void SortedList::updateItemAt(int index, h7::SortedList::ItemDelegate *item) {
        throwIfInMutationOperation();
        const h7::SortedList::ItemDelegate * existing = get(index);
        // assume changed if the same object is given back
        bool contentsChanged = existing == item || !mCallback->areContentsTheSame(existing, item);
        if (existing != item) {
            // different items, we can use comparison and may avoid lookup
            const int cmp = mCallback->compare(existing, item);
            if (cmp == 0) {
                mData[index] = item;
                if (contentsChanged) {
                    mCallback->onChanged(index, 1, mCallback->getChangePayload(existing, item));
                }
                return;
            }
        }
        if (contentsChanged) {
            mCallback->onChanged(index, 1, mCallback->getChangePayload(existing, item));
        }
        // TODO this done in 1 pass to avoid shifting twice.
        removeItemAtIndex(index, false);
        int newIndex = add(item, false);
        if (index != newIndex) {
            mCallback->onMoved(index, newIndex);
        }
    }

    void SortedList::recalculatePositionOfItemAt(int index) {
        throwIfInMutationOperation();
        // TODO can be improved
        const h7::SortedList::ItemDelegate * item = get(index);
        removeItemAtIndex(index, false);
        int newIndex = add(const_cast<ItemDelegate *>(item), false);
        if (index != newIndex) {
            mCallback->onMoved(index, newIndex);
        }
    }
    const SortedList::ItemDelegate* SortedList::get(int index) {
        if (index >= mSize || index < 0) {
            LOGE("Asked to get item at %d, but size is %d.", index, mSize);
            return nullptr;
        }
        if (!mOldData.isEmpty()) {
            // The call is made from a callback during addAll execution. The data is split
            // between mData and mOldData.
            if (index >= mNewDataStart) {
                return mOldData[index - mNewDataStart + mOldDataStart].get();
            }
        }
        return mData[index].get();
    }
    int SortedList::indexOf(const h7::SortedList::ItemDelegate *item) {
        if (!mOldData.isEmpty()) {
            int index = findIndexOf(const_cast<ItemDelegate *>(item), mData, 0, mNewDataStart, LOOKUP);
            if (index != INVALID_POSITION) {
                return index;
            }
            index = findIndexOf(const_cast<ItemDelegate *>(item), mOldData, mOldDataStart, mOldDataSize, LOOKUP);
            if (index != INVALID_POSITION) {
                return index - mOldDataStart + mNewDataStart;
            }
            return INVALID_POSITION;
        }
        return findIndexOf(const_cast<ItemDelegate *>(item), mData, 0, mSize, LOOKUP);
    }

    void SortedList::clear() {
        throwIfInMutationOperation();
        if (mSize == 0) {
            return;
        }
        const int prevSize = mSize;
        for (int i = 0; i < prevSize; ++i) {
            mData[i].reset();
        }
        mSize = 0;
        mCallback->onRemoved(0, prevSize);
    }
}