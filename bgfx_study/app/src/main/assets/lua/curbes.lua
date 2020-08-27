local bgfx = require("bgfx");
local bx = require("bx");

local initializer = bgfx.getInit();
local reso = initializer.resolution;

--------- fields ----------
local m_timeOffset;
local m_pt = 1;

local m_r = true;
local m_g = true;
local m_b = true;
local m_a = true;

local m_vbh;
local m_ibh = {};
local m_program;

------------ bgfx  ----------------
local ms_layout = bgfx.newVertexLayout();
local s_cubeVertices = bgfx.newMemoryFFFUI( {-1.0,  1.0,  1.0, 0xff000000},
        { 1.0,  1.0,  1.0, 0xff0000ff },
        {-1.0, -1.0,  1.0, 0xff00ff00 },
        { 1.0, -1.0,  1.0, 0xff00ffff },
        {-1.0,  1.0, -1.0, 0xffff0000 },
        { 1.0,  1.0, -1.0, 0xffff00ff },
        {-1.0, -1.0, -1.0, 0xffffff00 },
        { 1.0, -1.0, -1.0, 0xffffffff });

local s_cubeTriList = bgfx.newMemory('w', {
    0, 1, 2, -- 0
1, 3, 2,
4, 6, 5, -- 2
5, 6, 7,
0, 2, 4, -- 4
4, 2, 6,
1, 5, 3, -- 6
5, 7, 3,
0, 4, 1, -- 8
4, 5, 1,
2, 3, 6, -- 10
6, 3, 7,
})

local s_cubeTriStrip = bgfx.newMemory('w', {
    0, 1, 2,
    3,
    7,
    1,
    5,
    0,
    4,
    2,
    6,
    7,
    4,
    5,
})
local s_cubeLineList = bgfx.newMemory('w', {
    0, 1,
    0, 2,
    0, 4,
    1, 3,
    1, 5,
    2, 3,
    2, 6,
    3, 7,
    4, 5,
    4, 6,
    5, 7,
    6, 7,
})
local s_cubeLineStrip = bgfx.newMemory("w", {
    0, 2, 3, 1, 5, 7, 6, 4,
    0, 2, 6, 4, 5, 7, 3, 1,
    0,
})
local s_cubePoints = bgfx.newMemory('w', {
    0, 1, 2, 3, 4, 5, 6, 7
})
local s_ptNames = {
    "Triangle List",
    "Triangle Strip",
    "Lines",
    "Line Strip",
    "Points",
};
---  UINT64_C(0),
--                BGFX_STATE_PT_TRISTRIP,
--                BGFX_STATE_PT_LINES,
--                BGFX_STATE_PT_LINESTRIP,
--                BGFX_STATE_PT_POINTS,
local s_ptState = {
    0,
    0x0001000000000000,
    0x0002000000000000,
    0x0003000000000000,
    0x0004000000000000,
};
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
    -- init layout
    ms_layout
            .begin()
            .add("Position", 3, "Float")
            .add("Color0",   4, "Uint8", true)
            .ends();
    m_vbh = bgfx.createVertexBuffer(bgfx.makeRef(s_cubeVertices), ms_layout);
    m_ibh[1] = bgfx.createIndexBuffer(bgfx.makeRef(s_cubeTriList));
    m_ibh[2] = bgfx.createIndexBuffer(bgfx.makeRef(s_cubeTriStrip));
    m_ibh[3] = bgfx.createIndexBuffer(bgfx.makeRef(s_cubeLineList));
    m_ibh[4] = bgfx.createIndexBuffer(bgfx.makeRef(s_cubeLineStrip));
    m_ibh[5] = bgfx.createIndexBuffer(bgfx.makeRef(s_cubePoints));

    m_program = bgfx.loadProgram("vs_cubes", "fs_cubes");

    m_timeOffset = bx.getHPCounter();
    print("app_init done");
end
local app_draw = function ()
    print("app_draw");
    local m_width = reso.width;
    local m_height = reso.height;
    local time = (bx.getHPCounter()-m_timeOffset)*1.0/bx.getHPFrequency();
    --todo wait curbes draw

    bgfx.frame();
    return 0;
end
local app_destroy = function ()
    print("app_destroy");
end

local app = bgfx.newApp(app_pre_init, app_init, app_draw, app_destroy);
app.start(app);