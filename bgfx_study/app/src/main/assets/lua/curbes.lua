local bgfx = require("bgfx");

local initializer = bgfx.getInit();
local reso = initializer.resolution;

--------- fields ----------
local m_timeOffset;
local m_pt = 1;

local m_r = true;
local m_g = true;
local m_b = true;
local m_a = true;
------------ bgfx  ----------------
local ms_layout = bgfx.newVertexLayout();
---------------------------

local app_pre_init = function()
    print("app_pre_init");
    initializer.type = "Count";
    initializer.vendorId = 0;
    reso.reset = 0x00000080; --vsync
    print('end app_pre_init ----- ')
end
local app_init = function ()
    print("app_init");
    bgfx.setDebug(0); -- BGFX_DEBUG_NONE
    bgfx.setViewClear(0
    , 3          -- BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
    , 0x303030ff
    , 1.0
    , 0
    );
    ms_layout
            .begin()
            .add("Position", 3, "Float")
            .add("Color0",   4, "Uint8", true)
            .ends();
    print("app_init done");
end
local app_draw = function ()
    print("app_draw");

    bgfx.frame();
    return 0;
end
local app_destroy = function ()
    print("app_destroy");
end

local app = bgfx.newApp(app_pre_init, app_init, app_draw, app_destroy);
app.start(app);