---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/10/10 0010 下午 5:38
---
local mem = require("memory")
local m, r;
m = mem.new('i', {1, 2, 3}, {-4, -5, -6})

print("------- start test SkMemory -> extract ------ ")
r = m.extract(2, 5);
print("extract", r)
assert(#r == 3)
assert(r[0] == 3)
assert(r[1] == -4)
assert(r[2] == -5)

print("------- start test SkMemory -> kickOut ------ ")
r = m.kickOut(2);
print("kickOut", r)
assert(#r == 5)
assert(r[0] == 1)
assert(r[1] == 2)
assert(r[2] == -4)
assert(r[3] == -5)
assert(r[4] == -6)