local bgfx = require("bgfx");
local bx = require("bx");
local mem = require("memory");
local lib64 = require("uint64");
local int64 = lib64.new;

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
local s_cubeVertices = mem.newMemoryFFFUI( {-1.0,  1.0,  1.0, 0xff000000},
        { 1.0,  1.0,  1.0, 0xff0000ff },
        {-1.0, -1.0,  1.0, 0xff00ff00 },
        { 1.0, -1.0,  1.0, 0xff00ffff },
        {-1.0,  1.0, -1.0, 0xffff0000 },
        { 1.0,  1.0, -1.0, 0xffff00ff },
        {-1.0, -1.0, -1.0, 0xffffff00 },
        { 1.0, -1.0, -1.0, 0xffffffff });

local s_cubeTriList = mem.newMemory('w', {
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

local s_cubeTriStrip = mem.newMemory('w', {
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
local s_cubeLineList = mem.newMemory('w', {
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
local s_cubeLineStrip = mem.newMemory("w", {
    0, 2, 3, 1, 5, 7, 6, 4,
    0, 2, 6, 4, 5, 7, 3, 1,
    0,
})
local s_cubePoints = mem.newMemory('w', {
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
    int64(0x0001000000000000),
    int64(0x0002000000000000),
    int64(0x0003000000000000),
    int64(0x0004000000000000),
};

local BGFX_STATE_WRITE_R = int64(0x0000000000000001);
local BGFX_STATE_WRITE_G = int64(0x0000000000000002);
local BGFX_STATE_WRITE_B = int64(0x0000000000000004);
local BGFX_STATE_WRITE_A = int64(0x0000000000000008);
local BGFX_STATE_WRITE_Z = int64(0x0000004000000000);
local BGFX_STATE_DEPTH_TEST_LESS = int64(0x0000000000000010);
local BGFX_STATE_CULL_CW =  int64(0x0000001000000000);
local BGFX_STATE_CULL_CCW = int64(0x0000002000000000);
local BGFX_STATE_MSAA = int64(0x0100000000000000);
---------------------------
---
local app;
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
    print("app_init done, m_timeOffset = ", m_timeOffset);
end

local logFirst = true;
--local time = 0;
local app_draw = function ()
    print("app_draw");
    local m_width = reso.width;
    local m_height = reso.height;

    --time = time + 1.0/60.0;
    local time = (bx.getHPCounter()-m_timeOffset)*1.0/bx.getHPFrequency();
    if(logFirst) then
        print("time: ", time)
    end
    --
    local at = bx.newVec3(0, 0, 0);
    local eye = bx.newVec3(0, 0, -35);

    --Set view and projection matrix for view 0.
    local view = mem.newMemory('f', 16);
    bx.mtxLookAt(view, eye, at);
    if(logFirst) then
        print("mtxLookAt: "..tostring(view));
    end

    local proj = mem.newMemory('f', 16);
    --bx::mtxProj(proj, 60.0f, float(m_width)/float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bx.mtxProj(proj, 60, m_width / m_height, 0.1, 100, bgfx.getCaps().homogeneousDepth);
    if(logFirst) then
        print("mtxProj: "..tostring(proj));
    end

    bgfx.setViewTransform(0, view, proj);
    --Set view 0 default viewport.
    bgfx.setViewRect(0, 0, 0, m_width, m_height);

    -- This dummy draw call is here to make sure that view 0 is cleared
    -- if no other draw calls are submitted to view 0.
    bgfx.touch(0);
    local ibh = m_ibh[m_pt];

    local state = int64(0) | (m_r and BGFX_STATE_WRITE_R or 0)
         | (m_g and BGFX_STATE_WRITE_G or 0)
         | (m_b and BGFX_STATE_WRITE_B or 0)
         | (m_a and BGFX_STATE_WRITE_A or 0)
         | BGFX_STATE_WRITE_Z
         | BGFX_STATE_DEPTH_TEST_LESS
         | BGFX_STATE_CULL_CW
         | BGFX_STATE_MSAA
         | s_ptState[m_pt]
    --print("state", state)
    if(logFirst) then
        print("state: "..tostring(state));
    end

    if(logFirst) then
        print("----- start 11*11 curbes -----\n")
    end
    for y = 1, 11, 1 do
        local yy = y - 1;
        for x = 1, 11, 1 do
            --print("y, x = ", y, x)
            local xx = x - 1;
            local mtx = mem.newMemory('f', 16);
            --print("pre mtxRotateXY: "..tostring(mtx));
            bx.mtxRotateXY(mtx, time + xx*0.21, time + yy*0.37);
            --
            mtx[12] = -15.0 + xx*3.0;
            mtx[13] = -15.0 + yy*3.0;
            mtx[14] = 0.0;
            if(logFirst) then
                print("(xx, yy) = (".. xx..", "..yy..")")
                print("mtxRotateXY: "..tostring(mtx));
            end

            -- Set model matrix for rendering.
            bgfx.setTransform(mtx);

            -- Set vertex and index buffer.
            bgfx.setVertexBuffer(0, m_vbh);
            bgfx.setIndexBuffer(ibh);

            -- Set render states.
            bgfx.setState(state);

            -- Submit primitive for rendering to view 0.
            bgfx.submit(0, m_program);
        end
    end
    if(logFirst) then
        print("----- end 11*11 curbes -----\n")
    end
    logFirst = nil;
    -- Advance to next frame. Rendering thread will be kicked to
    -- process submitted rendering primitives.
    bgfx.frame();
    return 0;
end
local app_destroy = function ()
    print("app_destroy");
    -- [1~5]
    for i = 1, #m_ibh  do
        bgfx.destroy(m_ibh[i])
        bgfx.destroy(m_vbh);
        bgfx.destroy(m_program);
    end
end

app = bgfx.newApp(app_pre_init, app_init, app_draw, app_destroy);
app.start(app);