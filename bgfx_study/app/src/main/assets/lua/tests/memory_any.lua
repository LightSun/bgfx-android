---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/9/11 0011 下午 3:13
---
local mem = require("memory")
local me;
--me = mem.new("fdwbiscF", {1.1,2,3,4,5,6,7,8.88}, {11,12,13,14,15,16,17,18})
print("me = ", me)

--me = mem.new("fdwbiscF", {1.1,4294967295,65535,255, 2147483647,32767,127,8.88});
print("max range: me = ", me)

--me = mem.new("fdwbiscF", {1.1,4294967295+1,65535+1,255+1, 2147483647+1,32767+1,127+1,8.88});
print("over max range: me = ", me)

me = mem.new("fdwbiscF", {0,0,0,0, -2147483648,-32768,-128,8.88});
local a = -128;
print("min range: me = ", me, a)

--me = mem.new("fdwbiscF", {0,-1, -1, -1, -2147483648-1,-32768-1,-128-1,8.88});
print("less min range: me = ", me)