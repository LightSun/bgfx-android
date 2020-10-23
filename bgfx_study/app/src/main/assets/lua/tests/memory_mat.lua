---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/8/31 0031 下午 2:09
---

local mem = require("memory")

--- test transpose
local mat = mem.newMatrix('d', { 1, 2, 3, 4}, { 5, 6, 7, 8}, {9, 10 , 11, 12});
print("mat->transpose before", mat);
print("mat.len", #mat)
print(mat[0][0]);

mat[0][0] = 100;
print(mat[0][0]);

print("--- start test mat.transpose() ---------")
mat = mat.transpose();
print("mat->transpose", mat)
print(mat.isValid())
print("mat.len", #mat)

print("------ start test mat.convert() -------- ")
mat = mem.newMatrix('f', {1.1, 2.2, 3.3}, {1, 2.5, 8});
local mat2 = mat.convert('w');
print("mat->convert", mat2)

mat = mat.convert('dFf');
print("mat->convert", mat);
assert(mat.a == nil) -- no a member. expect

print("------ start test mat.extract() -------- ")
mat = mem.newMatrix('f', {1.1, 2.2, 3.3}, {1, 2.5, 8});
-- extractMat can have 1-4 arguments. size_t rowStart(1), size_t rowEnd(3), size_t columnStart(2),
--                                       size_t columnEnd(4)
assert(mat.extractMat(0) == mat)
assert(mat.extractMat(1) == mem.newMatrix('f', {1, 2.5, 8}))
assert(mat.extractMat(1, 1) == mem.newMatrix('f', {2.5, 8}))
assert(mat.extractMat(0, 1, 1) == mem.newMatrix('f', {2.2, 3.3}))
assert(mat.extractMat(0, 1, 0, 2) == mem.newMatrix('f', {1.1, 2.2}))