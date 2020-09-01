---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/8/31 0031 下午 2:09
---

local mem = require("memory")

local s_cubeVertices = mem.newMemoryFFFUI(
        {-1.0,  1.0,  1.0, 0xff000000 },
        { 1.0,  1.0,  1.0, 0xff0000ff },
        {-1.0, -1.0,  1.0, 0xff00ff00 },
        { 1.0, -1.0,  1.0, 0xff00ffff },
        {-1.0,  1.0, -1.0, 0xffff0000 },
        { 1.0,  1.0, -1.0, 0xffff00ff },
        {-1.0, -1.0, -1.0, 0xffffff00 },
        { 1.0, -1.0, -1.0, 0xffffffff });


assert(s_cubeVertices[0] == -1)
assert(s_cubeVertices[1] == 1)
assert(s_cubeVertices[2] == 1)
assert(s_cubeVertices[3] == 0xff000000)

assert(s_cubeVertices[4] == 1)
assert(s_cubeVertices[5] == 1)
assert(s_cubeVertices[6] == 1)
assert(s_cubeVertices[7] == 0xff0000ff)

assert(s_cubeVertices[8] == -1)
assert(s_cubeVertices[9] == -1)
assert(s_cubeVertices[10] == 1)
assert(s_cubeVertices[11] == 0xff00ff00)

assert(s_cubeVertices[12] == 1)
assert(s_cubeVertices[13] == -1)
assert(s_cubeVertices[14] == 1)
assert(s_cubeVertices[15] == 0xff00ffff)

assert(s_cubeVertices[16] == -1)
assert(s_cubeVertices[17] == 1)
assert(s_cubeVertices[18] == -1)
assert(s_cubeVertices[19] == 0xffff0000)

assert(s_cubeVertices[20] == 1)
assert(s_cubeVertices[21] == 1)
assert(s_cubeVertices[22] == -1)
assert(s_cubeVertices[23] == 0xffff00ff)

assert(s_cubeVertices[24] == -1)
assert(s_cubeVertices[25] == -1)
assert(s_cubeVertices[26] == -1)
assert(s_cubeVertices[27] == 0xffffff00)

assert(s_cubeVertices[28] == 1)
assert(s_cubeVertices[29] == -1)
assert(s_cubeVertices[30] == -1)
assert(s_cubeVertices[31] == 0xffffffff)
print(tostring(s_cubeVertices))

local t =  {
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
};
local aa = mem.newMemory('w', t);
for i = 1, #t do
    assert(aa[i-1] == t[i]);
end
--print(tostring(aa))

local m2 = mem.newMemory('b',  {1, 2, 3}, {4, 5, 6});
print("m2: ",tostring(m2))


local mat = mem.newMemoryMatrix('b', {1, 2, 3}, {4, 5, 6});
assert(#mat == 2)
assert(#(mat[0]) == 3)
assert(#(mat[1]) == 3)

assert(mat[0][0] == 1)
assert(mat[0][1] == 2)
assert(mat[0][2] == 3)

assert(mat[1][0] == 4)
assert(mat[1][1] == 5)
assert(mat[1][2] == 6)
print("tostring(mat)----------")
--print(tostring(mat))
--mem.newMemoryMatrix('d', 4, 4);
