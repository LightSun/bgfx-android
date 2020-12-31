//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_LISTLAYOUT_H
#define BGFX_STUDY_LISTLAYOUT_H

#include <map>
#include "../WidgetGroup.h"
#include "utils/SortedList.h"
#include "../../utils/SortedList.h"

namespace h7{
    class ListLayout;
    class ListItemDelegate;
    class ItemViewHolder;
    class LayoutManager;

    class ListAdapter: public SkRefCnt{
    public:
        virtual int getItemCount(){
            return 0;
        }
        virtual ListItemDelegate* getItem(int position){
            return nullptr;
        };
        virtual int getViewType(int pos, ListItemDelegate* item){
            return 0;
        }
        virtual ItemViewHolder* createViewHolder(const ListLayout* parent, int viewType, ListItemDelegate* item){
            return nullptr;
        }
        virtual void onBindData(ItemViewHolder* holder, int position){

        }
    };
    class ListItemDelegate: public SortedList::ItemDelegate{
    public:

    };
    class ItemViewHolder: public SkRefCnt{
    public:
        static constexpr int NO_POSITION = -1;
        sk_sp<Actor> itemView;
    private:
        int mPosition = NO_POSITION;
        int mOldPosition = NO_POSITION;
    };

    class LayoutManager: public SkRefCnt {
    public:
        virtual void layoutChildren(ListLayout* layout, float targetX, float targetY, float w, float h) {
        }
        virtual void measure(ListLayout* layout, float& w, float& h){
        }
        /**
       * Scroll horizontally by dx pixels in screen coordinates and return the distance traveled.
       * The default implementation does nothing and returns 0.
       *
       * @param dx            distance to scroll by in pixels. X increases as scroll position
       *                      approaches the right.
       * @param recycler      Recycler to use for fetching potentially cached views for a
       *                      position
       * @param state         Transient state of RecyclerView
       * @return The actual distance scrolled. The return value will be negative if dx was
       * negative and scrolling proceeeded in that direction.
       * <code>Math.abs(result)</code> may be less than dx if a boundary was reached.
       */
        virtual int scrollHorizontallyBy(int dx) {
            return 0;
        }

        /**
         * Scroll vertically by dy pixels in screen coordinates and return the distance traveled.
         * The default implementation does nothing and returns 0.
         *
         * @param dy            distance to scroll in pixels. Y increases as scroll position
         *                      approaches the bottom.
         * @param recycler      Recycler to use for fetching potentially cached views for a
         *                      position
         * @param state         Transient state of RecyclerView
         * @return The actual distance scrolled. The return value will be negative if dy was
         * negative and scrolling proceeeded in that direction.
         * <code>Math.abs(result)</code> may be less than dy if a boundary was reached.
         */
        virtual int scrollVerticallyBy(int dy) {
            return 0;
        }

        /**
         * Query if horizontal scrolling is currently supported. The default implementation
         * returns false.
         *
         * @return True if this LayoutManager can scroll the current contents horizontally
         */
        virtual bool canScrollHorizontally() {
            return false;
        }

        /**
         * Query if vertical scrolling is currently supported. The default implementation
         * returns false.
         *
         * @return True if this LayoutManager can scroll the current contents vertically
         */
        virtual bool canScrollVertically() {
            return false;
        }

        /**
         * Scroll to the specified adapter position.
         *
         * Actual position of the item on the screen depends on the LayoutManager implementation.
         * @param position Scroll to this adapter position.
         */
        virtual void scrollToPosition(int position) {

        }

        /**
         * <p>Smooth scroll to the specified adapter position.</p>
         * <p>To support smooth scrolling, override this method, create your {@link SmoothScroller}
         * instance and call {@link #startSmoothScroll(SmoothScroller)}.
         * </p>
         * @param recyclerView The RecyclerView to which this layout manager is attached
         * @param state    Current State of RecyclerView
         * @param position Scroll to this adapter position.
         */
        virtual void smoothScrollToPosition(ListLayout* layout,int position) {
        }
    };
    /**
     * 1, cache view item count.
     */
    class ListLayout: public WidgetGroup{

    protected:
        virtual void onLayoutChildren(float targetX, float targetY, float w, float h);

        virtual void measure(float& outW, float& outH);
    public:
        void setAdapter(ListAdapter* _adapter);
        void setLayoutManager(LayoutManager* _m);

        virtual void notifyItemInserted(int position){

        }

        virtual void notifyItemChanged(int position){

        }

        virtual void notifyItemRemoved(int position){

        }

        virtual void notifyItemMoved(int fromPosition, int toPosition){

        }

        virtual void notifyItemRangeChanged(int positionStart, int itemCount){

        }

        virtual void notifyItemRangeInserted(int positionStart, int itemCount){

        }

        virtual void notifyItemRangeRemoved(int positionStart, int itemCount){

        }
        const ListAdapter* getAdapter(){
            return adapter.get();
        }

        ItemViewHolder* findViewHolder(int viewType);

    private:
         void setUpByAdapter();

         sk_sp<ListAdapter> adapter;
         sk_sp<LayoutManager> layoutManager;
         std::map<unsigned char, sk_sp<ItemViewHolder>> holderMap;
         //SortedList list;
    };
}

#endif //BGFX_STUDY_LISTLAYOUT_H
