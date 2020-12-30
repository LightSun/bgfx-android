//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_SORTEDLIST_H
#define BGFX_STUDY_SORTEDLIST_H

#include "../lua/SkRefCnt.h"
#include "Array.h"
#include "ListUpdateCallback.h"
#include "BatchingListUpdateCallback.h"
#include "Comparator.h"

namespace h7 {

    /**
 * A Sorted list implementation that can keep items in order and also notify for changes in the
 * list
 * such that it can be bound to a {@link RecyclerView.Adapter
 * RecyclerView.Adapter}.
 * <p>
 * It keeps items ordered using the {@link Callback#compare(Object, Object)} method and uses
 * binary search to retrieve items. If the sorting criteria of your items may change, make sure you
 * call appropriate methods while editing them to avoid data inconsistencies.
 * <p>
 * You can control the order of items and change notifications via the {@link Callback} parameter.
 */
    class SortedList {
        /**
    * Used by {@link #indexOf(Object)} when he item cannot be found in the list.
    */
    public:
        static constexpr int INVALID_POSITION = -1;

        class ItemDelegate : public SkRefCnt {

        public:
            //default empty.
            virtual ItemDelegate *copy() {
                return this;
            }
        };

        class Callback : public ListUpdateCallback, public Comparator<ItemDelegate> {
        public:

            virtual bool isBatch() {
                return false;
            }

            /**
             * Similar to {@link java.util.Comparator#compare(Object, Object)}, should compare two and
             * return how they should be ordered.
             *
             * @param o1 The first object to compare.
             * @param o2 The second object to compare.
             *
             * @return a negative integer, zero, or a positive integer as the
             * first argument is less than, equal to, or greater than the
             * second.
             */
            virtual int compare(const ItemDelegate *o1, const ItemDelegate *o2) { return 0; }

            /**
             * Called by the SortedList when the item at the given position is updated.
             *
             * @param position The position of the item which has been updated.
             * @param count    The number of items which has changed.
             */
            virtual void onChanged(int position, int count) {}

            virtual void onChanged(int position, int count, void *payload) {
                onChanged(position, count);
            }

            /**
             * Called by the SortedList when it wants to check whether two items have the same data
             * or not. SortedList uses this information to decide whether it should call
             * {@link #onChanged(int, int)} or not.
             * <p>
             * SortedList uses this method to check equality instead of {@link Object#equals(Object)}
             * so
             * that you can change its behavior depending on your UI.
             * <p>
             * For example, if you are using SortedList with a
             * {@link RecyclerView.Adapter RecyclerView.Adapter}, you should
             * return whether the items' visual representations are the same or not.
             *
             * @param oldItem The previous representation of the object.
             * @param newItem The new object that replaces the previous one.
             *
             * @return True if the contents of the items are the same or false if they are different.
             */
            virtual bool
            areContentsTheSame(const ItemDelegate *oldItem,
                               const ItemDelegate *newItem) { //onChanged
                return false;
            }

            /**
             * Called by the SortedList to decide whether two objects represent the same Item or not.
             * <p>
             * For example, if your items have unique ids, this method should check their equality.
             *
             * @param item1 The first item to check.
             * @param item2 The second item to check.
             *
             * @return True if the two items represent the same object or false if they are different.
             */
            virtual bool areItemsTheSame(const ItemDelegate *item1, const ItemDelegate *item2) {
                return false;
            }

            /**
             * When {@link #areItemsTheSame(ItemDelegate&, ItemDelegate&)} returns {@code true} for two items and
             * {@link #areContentsTheSame(ItemDelegate&, ItemDelegate&)} returns false for them, {@link Callback} calls this
             * method to get a payload about the change.
             * <p>
             * For example, if you are using {@link Callback} with
             * {@link RecyclerView}, you can return the particular field that
             * changed in the item and your
             * {@link RecyclerView.ItemAnimator ItemAnimator} can use that
             * information to run the correct animation.
             * <p>
             * Default implementation returns {@code null}.
             *
             * @param item1 The first item to check.
             * @param item2 The second item to check.
             * @return A payload object that represents the changes between the two items.
             */
            virtual void *getChangePayload(const ItemDelegate *item1, const ItemDelegate *item2) {
                return nullptr;
            }
        };

        class BatchedCallback : public Callback {
        public:
            BatchedCallback(sk_sp<Callback> base) {
                mWrappedCallback = base;
                const sk_sp<ListUpdateCallback> &sp = sk_ref_sp(
                        rCast(ListUpdateCallback*, base.get()));
                mBatchingListUpdateCallback.reset(new BatchingListUpdateCallback(sp));
            }

            virtual bool isBatch() {
                return true;
            }

            virtual int compare(const ItemDelegate *o1, const ItemDelegate *o2) {
                return mWrappedCallback->compare(o1, o2);
            }

            virtual void onInserted(int position, int count) {
                mBatchingListUpdateCallback->onInserted(position, count);
            }

            virtual void onRemoved(int position, int count) {
                mBatchingListUpdateCallback->onRemoved(position, count);
            }

            virtual void onMoved(int fromPosition, int toPosition) {
                mBatchingListUpdateCallback->onMoved(fromPosition, toPosition);
            }

            virtual void onChanged(int position, int count) {
                mBatchingListUpdateCallback->onChanged(position, count, nullptr);
            }

            virtual void onChanged(int position, int count, void *payload) {
                mBatchingListUpdateCallback->onChanged(position, count, payload);
            }

            virtual bool
            areContentsTheSame(const ItemDelegate *oldItem, const ItemDelegate *newItem) {
                return mWrappedCallback->areContentsTheSame(oldItem, newItem);
            }

            virtual bool areItemsTheSame(const ItemDelegate *item1, const ItemDelegate *item2) {
                return mWrappedCallback->areItemsTheSame(item1, item2);
            }

            virtual void *getChangePayload(const ItemDelegate *item1, const ItemDelegate *item2) {
                return mWrappedCallback->getChangePayload(item1, item2);
            }

            /**
             * This method dispatches any pending event notifications to the wrapped Callback.
             * You <b>must</b> always call this method after you are done with editing the SortedList.
             */
            virtual void dispatchLastEvent() {
                mBatchingListUpdateCallback->dispatchLastEvent();
            }

        public:
            sk_sp<Callback> mWrappedCallback;
        private:
            sk_sp<BatchingListUpdateCallback> mBatchingListUpdateCallback;
        };

    private:
        static constexpr int MIN_CAPACITY = 10;
        static constexpr int CAPACITY_GROWTH = MIN_CAPACITY;
        static constexpr int INSERTION = 1;
        static constexpr int DELETION = 1 << 1;
        static constexpr int LOOKUP = 1 << 2;

        /**
   * The current index into mOldData that has not yet been processed during a mutation operation
   * (addAll or replaceAll).
   */
        int mOldDataStart;
        int mOldDataSize;

        /**
         * The current index into the new data that has not yet been processed during a mutation
         * operation (addAll or replaceAll).
         */
        int mNewDataStart;

        int mSize;

        Array<sk_sp<ItemDelegate>> mData;
        /**
     * A reference to the previous set of data that is kept during a mutation operation (addAll or
     * replaceAll).
     */
        Array<sk_sp<ItemDelegate>> mOldData;

        /**
         * The callback instance that controls the behavior of the SortedList and get notified when
         * changes happen.
         */
        sk_sp<Callback> mCallback;
        sk_sp<BatchedCallback> mBatchedCallback;

        /**
     * Throws an exception if called while we are in the middle of a mutation operation (addAll or
     * replaceAll).
     */
        void throwIfInMutationOperation();

        /**
            * Sorts and removes duplicate items, leaving only the last item from each group of "same"
            * items. Move the remaining items to the beginning of the array.
            *
            * @return Number of deduplicated items at the beginning of the array.
            */
        int sortAndDedup(Array<sk_sp<ItemDelegate>> &items);

        int findSameItem(ItemDelegate *item, Array<sk_sp<ItemDelegate>> &items, int from, int to);

        void addAllInternal(Array<sk_sp<ItemDelegate>> &items);

        /**
        * This method assumes that newItems are sorted and deduplicated.
        */
        void merge(Array<sk_sp<ItemDelegate>> &newData, int newDataSize);

        int add(ItemDelegate *item, bool notify);

        void removeItemAtIndex(int index, bool notify);

        bool remove(ItemDelegate *item, bool notify);

        int findIndexOf(ItemDelegate *item, Array<sk_sp<ItemDelegate>> &mData, int left, int right,
                        int reason);

        int linearEqualitySearch(ItemDelegate *item, int middle, int left, int right);

        void addToData(int index, ItemDelegate *item);

        void replaceAllInternal(Array<sk_sp<ItemDelegate>> &items);

        void replaceAllRemove();

        void replaceAllInsert(ItemDelegate *newItem);

    public:
        SortedList(Callback *callback, int initialCapacity);

        SortedList(Callback *callback);

        int size();

        /**
    * Batches adapter updates that happen after calling this method and before calling
    * {@link #endBatchedUpdates()}. For example, if you add multiple items in a loop
    * and they are placed into consecutive indices, SortedList calls
    * {@link Callback#onInserted(int, int)} only once with the proper item count. If an event
    * cannot be merged with the previous event, the previous event is dispatched
    * to the callback instantly.
    * <p>
    * After running your data updates, you <b>must</b> call {@link #endBatchedUpdates()}
    * which will dispatch any deferred data change event to the current callback.
    * <p>
    * A sample implementation may look like this:
    * <pre>
    *     mSortedList.beginBatchedUpdates();
    *     try {
    *         mSortedList.add(item1)
    *         mSortedList.add(item2)
    *         mSortedList.remove(item3)
    *         ...
    *     } finally {
    *         mSortedList.endBatchedUpdates();
    *     }
    * </pre>
    * <p>
    * Instead of using this method to batch calls, you can use a Callback that extends
    * {@link BatchedCallback}. In that case, you must make sure that you are manually calling
    * {@link BatchedCallback#dispatchLastEvent()} right after you complete your data changes.
    * Failing to do so may create data inconsistencies with the Callback.
    * <p>
    * If the current Callback is an instance of {@link BatchedCallback}, calling this method
    * has no effect.
    */
        void beginBatchedUpdates();

        /**
         * Ends the update transaction and dispatches any remaining event to the callback.
         */
        void endBatchedUpdates();

        /**
            * Adds the given item to the list. If this is a new item, SortedList calls
            * {@link Callback#onInserted(int, int)}.
            * <p>
            * If the item already exists in the list and its sorting criteria is not changed, it is
            * replaced with the existing Item. SortedList uses
            * {@link Callback#areItemsTheSame(Object, Object)} to check if two items are the same item
            * and uses {@link Callback#areContentsTheSame(Object, Object)} to decide whether it should
            * call {@link Callback#onChanged(int, int)} or not. In both cases, it always removes the
            * reference to the old item and puts the new item into the backing array even if
            * {@link Callback#areContentsTheSame(Object, Object)} returns false.
            * <p>
            * If the sorting criteria of the item is changed, SortedList won't be able to find
            * its duplicate in the list which will result in having a duplicate of the Item in the list.
            * If you need to update sorting criteria of an item that already exists in the list,
            * use {@link #updateItemAt(int, Object)}. You can find the index of the item using
            * {@link #indexOf(Object)} before you update the object.
            *
            * @param item The item to be added into the list.
            *
            * @return The index of the newly added item.
            * @see Callback#compare(Object, Object)
            * @see Callback#areItemsTheSame(Object, Object)
            * @see Callback#areContentsTheSame(Object, Object)}
            */
        int add(const ItemDelegate *item);

        /**
 * Adds the given items to the list. Equivalent to calling {@link SortedList#add} in a loop,
 * except the callback events may be in a different order/granularity since addAll can batch
 * them for better performance.
 * <p>
 * If allowed, will reference the input array during, and possibly after, the operation to avoid
 * extra memory allocation, in which case you should not continue to reference or modify the
 * array yourself.
 * <p>
 * @param items Array of items to be added into the list.
 * @param mayModifyInput If true, SortedList is allowed to modify and permanently reference the
 *                       input array.
 * @see SortedList#addAll(T[] items)
 */
        void addAll(Array<sk_sp<ItemDelegate>> &items, bool mayModifyInput);

        /**
        * Adds the given items to the list. Does not modify or retain the input.
        *
        * @see SortedList#addAll(T[] items, boolean mayModifyInput)
        *
        * @param items Array of items to be added into the list.
        */
        void addAll(int n, ...);

        /**
       * Adds the given items to the list. Does not modify or retain the input.
       *
       * @see SortedList#addAll(T[] items, boolean mayModifyInput)
       *
       * @param items Collection of items to be added into the list.
       */
        void addAll(Array<sk_sp<ItemDelegate>> &items);

        /**
     * Replaces the current items with the new items, dispatching {@link ListUpdateCallback} events
     * for each change detected as appropriate.
     * <p>
     * If allowed, will reference the input array during, and possibly after, the operation to avoid
     * extra memory allocation, in which case you should not continue to reference or modify the
     * array yourself.
     * <p>
     * Note: this method does not detect moves or dispatch
     * {@link ListUpdateCallback#onMoved(int, int)} events. It instead treats moves as a remove
     * followed by an add and therefore dispatches {@link ListUpdateCallback#onRemoved(int, int)}
     * and {@link ListUpdateCallback#onRemoved(int, int)} events.  See {@link DiffUtil} if you want
     * your implementation to dispatch move events.
     * <p>
     * @param items Array of items to replace current items.
     * @param mayModifyInput If true, SortedList is allowed to modify and permanently reference the
     *                       input array.
     * @see #replaceAll(T[])
     */
        void replaceAll(Array<sk_sp<ItemDelegate>> &items, bool mayModifyInput);

        /**
         * Replaces the current items with the new items, dispatching {@link ListUpdateCallback} events
         * for each change detected as appropriate.  Does not modify or retain the input.
         *
         * @see #replaceAll(T[], boolean)
         *
         * @param items Array of items to replace current items.
         */
        void replaceAll(Array<sk_sp<ItemDelegate>> &items);

        /**
         * Removes the provided item from the list and calls {@link Callback#onRemoved(int, int)}.
         *
         * @param item The item to be removed from the list.
         *
         * @return True if item is removed, false if item cannot be found in the list.
         */
        bool remove(const ItemDelegate *item);

        /**
        * Removes the item at the given index and calls {@link Callback#onRemoved(int, int)}.
        *
        * @param index The index of the item to be removed.
        *
        * @return The removed item.
        */
        const ItemDelegate *removeItemAt(int index);

        /**
   * Returns the item at the given index.
   *
   * @param index The index of the item to retrieve.
   *
   * @return The item at the given index.
   * @throws java.lang.IndexOutOfBoundsException if provided index is negative or larger than the
   *                                             size of the list.
   */
        const ItemDelegate *get(int index);

        /**
     * Updates the item at the given index and calls {@link Callback#onChanged(int, int)} and/or
     * {@link Callback#onMoved(int, int)} if necessary.
     * <p>
     * You can use this method if you need to change an existing Item such that its position in the
     * list may change.
     * <p>
     * If the new object is a different object (<code>get(index) != item</code>) and
     * {@link Callback#areContentsTheSame(Object, Object)} returns <code>true</code>, SortedList
     * avoids calling {@link Callback#onChanged(int, int)} otherwise it calls
     * {@link Callback#onChanged(int, int)}.
     * <p>
     * If the new position of the item is different than the provided <code>index</code>,
     * SortedList
     * calls {@link Callback#onMoved(int, int)}.
     *
     * @param index The index of the item to replace
     * @param item  The item to replace the item at the given Index.
     * @see #add(Object)
     */
        void updateItemAt(int index, ItemDelegate *item);

        /**
      * This method can be used to recalculate the position of the item at the given index, without
      * triggering an {@link Callback#onChanged(int, int)} callback.
      * <p>
      * If you are editing objects in the list such that their position in the list may change but
      * you don't want to trigger an onChange animation, you can use this method to re-position it.
      * If the item changes position, SortedList will call {@link Callback#onMoved(int, int)}
      * without
      * calling {@link Callback#onChanged(int, int)}.
      * <p>
      * A sample usage may look like:
      *
      * <pre>
      *     final int position = mSortedList.indexOf(item);
      *     item.incrementPriority(); // assume items are sorted by priority
      *     mSortedList.recalculatePositionOfItemAt(position);
      * </pre>
      * In the example above, because the sorting criteria of the item has been changed,
      * mSortedList.indexOf(item) will not be able to find the item. This is why the code above
      * first
      * gets the position before editing the item, edits it and informs the SortedList that item
      * should be repositioned.
      *
      * @param index The current index of the Item whose position should be re-calculated.
      * @see #updateItemAt(int, Object)
      * @see #add(Object)
      */
        void recalculatePositionOfItemAt(int index);

        /**
       * Returns the position of the provided item.
       *
       * @param item The item to query for position.
       *
       * @return The position of the provided item or {@link #INVALID_POSITION} if item is not in the
       * list.
       */
        int indexOf(const ItemDelegate *item);

        /**
      * Removes all items from the SortedList.
      */
        void clear();
    };
}

#endif //BGFX_STUDY_SORTEDLIST_H
