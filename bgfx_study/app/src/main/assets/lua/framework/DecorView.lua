---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/11/1 0001 下午 10:40
---
local m = {}

--config: renderType, renderState
function m.new(config, extra)
    local self = {};
    self.views = {};
    --todo self.canvas =

    function self.addView(view, index)
        if(not index) then
            index = #self.views;
        end
        table.insert(self.views, index, view);
    end
    function self.removeView(view)
        local index;
        for i, v in ipairs(self.views) do
            if v == view then
                index = i;
                break;
            end
        end
        table.remove(self.views, index)
    end
    function self.setView(index, view)
        self.views[index] = view;
    end

    function self.getExtra()
        return extra;
    end

    function self.onInitialize()

    end
    function self.onDestroy()

    end
    function self.onDraw()

    end
    return self;
end

return m;