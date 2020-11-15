---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/8/23 0023 下午 9:34
---
local m = {};
--require("bgfx_lua");
local func_wrap = require("func_wrap");
local ud_wrap = require("ud_wrap");

local function dumpInit(init_wrapper, out)
    out.type = init_wrapper.type();
    out.vendorId = init_wrapper.vendorId();
    out.deviceId = init_wrapper.deviceId();
    out.debug = init_wrapper.debug();
    out.profile = init_wrapper.profile();
    out.platformData = init_wrapper.platformData();
    out.resolution = init_wrapper.resolution();
    out.limits = init_wrapper.limits();
    return out;
end

function m.getInit()
    local init = bgfx_lua.getInit();
    return ud_wrap.wrapGetSet(init);
end

function m.startApp(func_pre_init, func_init, func_draw, func_destroy)
    return bgfx_lua.startApp(
            func_wrap.wrapEasy(func_pre_init, "app_pre_init"),
            func_wrap.wrapEasy(func_init, "app_init"),
            func_wrap.wrapEasy(func_draw, "app_draw"),
            func_wrap.wrapEasy(func_destroy, "app_destroy")
    );
end

function m.setDebug(debugFlags)
    return bgfx_lua.setDebug(debugFlags);
end

---
--- setViewClear
---  _id: the view id
--- c_flags: clear flags
--- c_rgba: clear rgba
--- c_depth: clear depth
--- c_stencil: clear stencil
function m.setViewClear(_id, c_flags, c_rgba, c_depth, c_stencil)
    --print("bgfx >>> setViewClear")
    bgfx_lua.setViewClear(_id, c_flags, c_rgba, c_depth, c_stencil);
end
function m.setViewRect(_id, x, y, w, h)
    --print("bgfx >>> setViewRect")
    bgfx_lua.setViewRect(_id, x, y, w, h);
end

function m.touch(id)
    --print("bgfx >>> touch")
    bgfx_lua.touch(id);
end

function m.dbgTextClear()
    --print("bgfx >>> dbgTextClear")
    bgfx_lua.dbgTextClear();
end

--- image: const char *
function m.dbgTextImage(x, y, w, h,image, pitch)
    --print("bgfx >>> dbgTextImage")
    bgfx_lua.dbgTextImage(x, y, w, h,image, pitch);
end

function m.dbgTextPrintf(x, y, attrib, format,...)
    local str = string.format(format, ...);
    print("bgfx >>> dbgTextPrintf, str =", str)
    bgfx_lua.dbgTextPrintf(x, y , attrib, str);
end

function m.frame(capture)
    bgfx_lua.frame(capture);
end

--- number of fields can access.
--- cpuTimeFrame, cpuTimeBegin, cpuTimeEnd, cpuTimerFreq
--- gpuTimeBegin, gpuTimeEnd, gpuTimerFreq
--- waitRender, waitSubmit
--- numDraw, numCompute, numBlit, maxGpuLatency
--- numDynamicIndexBuffers, numDynamicVertexBuffers, numFrameBuffers
--- numIndexBuffers, numOcclusionQueries,
--- numPrograms, numShaders, numTextures,numUniforms
--- numVertexBuffers, numVertexLayouts
--- textureMemoryUsed, rtMemoryUsed
--- transientVbUsed, transientIbUsed
--- gpuMemoryMax, gpuMemoryUsed
--- width, height, textWidth, textHeight
--- numViews, numEncoders
function m.getStats()
    return ud_wrap.wrapGet(bgfx_lua.getStats());
end

--- number of fields can access.
--- rendererType, homogeneousDepth, originBottomLeft, numGPUs, formats
function m.getCaps()
    return ud_wrap.wrapGet(bgfx_lua.getCaps());
end

--- new VertexLayout.
--- @return VertexLayout from bgfx pkg
function m.newVertexLayout()
    return ud_wrap.wrapCall(bgfx_lua.newVertexLayout(), nil);
end

---
---@return userdata of Memory
function m.makeRef(ud_mem)
    return bgfx_lua.makeRef(ud_mem);
end
--- create vertex buffer
--- @param mem_ref: the memory ref return from 'makeRef(ud_mem)'.
--- @param verLayout_ref: vertex layout ref
--- @param flags: the flags. default is 0
--- @return vertexBufferHandle
function m.createVertexBuffer(mem_ref, verLayout_ref, flags)
    if(not flags) then
        flags = 0;
    end
    -- for verLayout_ref often be wrapped. by 'ud_wrap.lua'.
    if(verLayout_ref._unwrap) then
        verLayout_ref = verLayout_ref._unwrap();
    end
    return bgfx_lua.createVertexBuffer(mem_ref, verLayout_ref, flags);
end
--- create vertex buffer
--- @param mem_ref: the memory ref return from 'makeRef(ud_mem)'.
--- @param verLayout_ref:
--- @param flags: the flags . default is 0
--- @return IndexBufferHandle
function m.createIndexBuffer(mem_ref, flags)
    if not flags then
        flags = 0;
    end
    return bgfx_lua.createIndexBuffer(mem_ref, flags);
end

---
--- createProgram
--- @param v_sh: ShaderHandle of vertex
--- @param f_sh: ShaderHandle of fragment
--- @param destroy_shader: true if destroy shader
--- @return ProgramHandle
function m.createProgram(v_sh, f_sh, destroy_shader)
    return bgfx_lua.createProgram(v_sh, f_sh, destroy_shader);
end
---
--- createProgram
--- @param v_sh: name of vertex shader
--- @param f_sh: name of fragment shader
--- @return ProgramHandle
function m.loadProgram(v_sh_name, f_sh_name)
    return bgfx_lua.loadProgram(v_sh_name, f_sh_name);
end

--- set setViewTransform
--- @param viewId: view id
--- @param mem_1: memory userdata. often is float type
--- @param mem_2: memory userdata. often is float type
function m.setViewTransform(viewId, mem_1, mem_2)
    return bgfx_lua.setViewTransform(viewId, mem_1, mem_2);
end

--- set transform
--- @param mem_or_cache: memory userdata or matrix cache(uint32)
--- @param _num: Number of matrices in array. default is 1
function m.setTransform(mem_or_cache, _num)
    if(not _num) then
        _num = 1;
    end
    return bgfx_lua.setTransform(mem_or_cache, _num);
end

--- setVertexBuffer
--- @param stream: uint16_t
--- @param vertexHandle: vertex handle userdata
function m.setVertexBuffer(stream, vertexHandle)
    return bgfx_lua.setVertexBuffer(stream, vertexHandle);
end
--- setIndexBuffer
--- @param indexHandle: index buffer handle userdata
function m.setIndexBuffer(indexHandle)
    return bgfx_lua.setIndexBuffer(indexHandle);
end

--- setState
--- @param _state: uint64_t(see uint64.lua)
--- @param _rgba: uint32_t. can be null for default 0.
function m.setState(_state, _rgba)
    return bgfx_lua.setState(_state, _rgba);
end

--- submit data
--- @param _id: view id
--- @param _program: program handle
--- @param _depth: depth as uint32_t.  null means default 0
--- @param _flags: flags as uint8_t. null means BGFX_DISCARD_ALL
function m.submit(_id, _program, _depth, _flags)
    return bgfx_lua.submit(_id, _program, _depth, _flags);
end

--- destroy handle
--- @param handle: can be userdata of IndexBufferHandle/VertexBufferHandle/ProgramHandle
function m.destroy(handle)
    return bgfx_lua.destroy(handle);
end

return m;