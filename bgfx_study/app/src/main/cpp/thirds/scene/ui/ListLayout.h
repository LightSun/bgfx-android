//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_LISTLAYOUT_H
#define BGFX_STUDY_LISTLAYOUT_H

#include "../WidgetGroup.h"

namespace h7{

    class ListAdapter{

    };
    class ListItemDelegate{
    public:

    };

    class ListLayout: public WidgetGroup{

    public:


    private:
         sk_sp<ListAdapter> sp_adapter;
         Array<sk_sp<ListItemDelegate>> _items;
    };
}

#endif //BGFX_STUDY_LISTLAYOUT_H
