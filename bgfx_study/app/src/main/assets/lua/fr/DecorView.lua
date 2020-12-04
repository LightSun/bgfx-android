---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/11/1 0001 下午 10:40
---
local obj = require("core.Object")
local gui = require("fr.UiCore")
local bgfx = require("bgfx");

local bx = require("bx");
local mem = require("memory");

local m = {}

--- create decor view with canvas config
--- canvas config: (viewId, width, height, [scaleRatio])
function m.new(canvasConfig)
    local self = obj.new("DecorView");
    self.views = {};
    if( canvasConfig and type(canvasConfig) == 'table') then
        self.canvas = gui.newCanvas(canvasConfig.viewId, canvasConfig.width, canvasConfig.height, canvasConfig.scaleRatio)
        self.viewId = canvasConfig.viewId;
    else
        self.viewId = 0;
        print("create DecorView with no config, latter will create canvas by default.")
    end

    function self.addView(view, index)
        if(not index) then
            index = #self.views;
        end
        table.insert(self.views, index + 1, view);
        view.onAttach(self)
    end
    function self.removeView(view)
        local index;
        for i, v in ipairs(self.views) do
            if v == view then
                index = i;
                break;
            end
        end
        if(index) then
            table.remove(self.views, index)
            view.onDetach(self);
        end
    end
    function self.removeViewAt(index)
        local view = table.remove(self.views, index + 1)
        if(view) then
            view.onDetach(self);
        end
    end
    function self.setView(index, view)
        local old = self.views[index + 1];
        self.views[index + 1] = view;
        if(old) then
            old.onDetach(self)
        end
        view.onAttach(self);
    end
    -- index from 0
    function self.getChildAt(index)
        return self.views[index + 1];
    end
    function self.getChildrenCount()
        return #self.views;
    end

    function self.getCanvas()
        if(not self.canvas) then
            local reso = bgfx.getInit().resolution
            self.canvas = gui.newCanvas(self.viewId, reso.width, reso.height, 1)
        end
        return self.canvas;
    end

    function self.onInitialize()
        bgfx.setDebug(0); -- BGFX_DEBUG_NONE
        bgfx.setViewClear(0
        , 3          -- BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
        , 0x303030ff
        , 1.0
        , 0);
        --dumpStack();
        local ctx = self.getCanvas().getContext()
        --dumpStack();
        for _, v in ipairs(self.views) do
            v.onInitialize(ctx);
        end
    end
    function self.onDestroy()
        for _, v in ipairs(self.views) do
            v.onDestroy();
        end
    end
    function self.onDraw()
        local canvas = self.canvas;
        local reso = bgfx.getInit().resolution
        bgfx.setViewRect(self.viewId, 0, 0, reso.width, reso.height);

        bgfx.touch(self.viewId);
        canvas.beginFrame(reso.width, reso.height);

        for _, v in ipairs(self.views) do
            v.onDraw(canvas);
        end
        --nanovg doesn't support setTransform.

        canvas.endFrame();
        bgfx.frame();
    end
    return self;
end

return m;