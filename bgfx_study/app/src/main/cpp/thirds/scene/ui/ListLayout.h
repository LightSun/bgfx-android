//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_LISTLAYOUT_H
#define BGFX_STUDY_LISTLAYOUT_H

#include "../WidgetGroup.h"
#include "utils/SortedList.h"
#include "../../utils/SortedList.h"

namespace h7{
    class ListLayout;
    class ListItemDelegate;
    class ItemViewHolder;

    class ListAdapter{
    public:
        virtual int getItemCount(){
            return 0;
        }
        virtual int getViewType(int pos, ListItemDelegate* item){
            return 0;
        }
        virtual ItemViewHolder* createViewHolder(const ListLayout* parent, int viewType, ListItemDelegate* item){
            return nullptr;
        }
    };
    class ListItemDelegate: public SortedList::ItemDelegate{
    public:

    };
    class ItemViewHolder{
    public:
        static constexpr int NO_POSITION = -1;
        Actor* itemView;
    private:
        int mPosition = NO_POSITION;
        int mOldPosition = NO_POSITION;
    };
    /**
     * 1, cache view item count.
     */
    class ListLayout: public WidgetGroup{

    public:
        virtual void onLayoutChildren(float targetX, float targetY, float w, float h);

        virtual void measure(float& outW, float& outH);

    private:
         ListAdapter* adapter;
         SortedList list;
    };
}

#endif //BGFX_STUDY_LISTLAYOUT_H
