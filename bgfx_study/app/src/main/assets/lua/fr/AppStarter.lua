---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/11/3 0003 上午 12:24
---
---
local bgfx = require("bgfx");
--local preds = require("core.utils.Predicates")
local dv = require("fr.DecorView")
local m = {};

-- decorViews
function m.start(decorViews, appConfig)
    -- renderType, debugFlags, reset(flags)
    if(not appConfig) then
        appConfig = {renderType='Count',vendorId=0, reset=0x00000080};
    end
    if(not decorViews or type(decorViews) ~= 'table') or (#decorViews == 0) then
        error("at least need one DecorView.");
    end
    local initializer = bgfx.getInit();
    local reso = initializer.resolution;
    local app_pre_init = function()
        initializer.type = appConfig.renderType;
        initializer.vendorId = appConfig.vendorId;
        reso.reset = appConfig.reset; --vsync
    end
    local app_init = function ()
        for _, v in ipairs(decorViews) do
            v.onInitialize();
        end
    end
    local app_draw = function ()
        for _, v in ipairs(decorViews) do
            v.onDraw();
        end
    end
    local app_destroy = function ()
        for _, v in ipairs(decorViews) do
            v.onDestroy();
        end
    end
    local app = bgfx.newApp(app_pre_init, app_init, app_draw, app_destroy);
    app.start(app);
end

--- start view with default canvas config. this often used for test.
function m.startViews(appConfig, ...)
    local views = {...}
    local decorView = dv.new();
    for _, v in ipairs(views) do
        decorView.addView(v)
    end
    local tab = {decorView}
    m.start(tab, appConfig);
end

return m;