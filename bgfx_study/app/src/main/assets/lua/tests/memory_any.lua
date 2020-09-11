---
--- this demo shows how to use memory data:  new/foreach/convert/isValid
--- memory support types:   f,     d,      w,    b,    i,     s,       c,         F
---                      float, uint32, uint16, uint8, int, short,  signed char, double
---
local mem = require("memory")
local me;
print("------------ start test value range of 'fdwbiscF' --------------")
me = mem.new("fdwbiscF", {1.1,2,3,4,5,6,7,8.88}, {11,12,13,14,15,16,17,18})
print("me = ", me)

me = mem.new("fdwbiscF", {1.1,4294967295,65535,255, 2147483647,32767,127,8.88});
print("max range: me = ", me)

me = mem.new("fdwbiscF", {1.1,4294967295+1,65535+1,255+1, 2147483647+1,32767+1,127+1,8.88});
print("over max range: me = ", me)

me = mem.new("fdwbiscF", {0,0,0,0, -2147483648,-32768,-128,8.88});
print("min range: me = ", me)

me = mem.new("fdwbiscF", {0,-1, -1, -1, -2147483648-1,-32768-1,-128-1,8.88});
print("less min range: me = ", me)

print("-------------- start test foreach ---------------")
me = mem.new('d', {1,2,3,4,5,6})
me.foreach(function (index, value)
    print("foreach: d", index, value);
end);

me = mem.new("fdwbiscF", {1.1,2,3,4,5,6,7,8.88});
me.foreach(function (index, value)
    print("foreach: fdwbiscF", index, value);
end);

print("---------------- start test convert -------------- ")
me = mem.new('d', {1,2,3,4,5,6})
me = me.convert('fff')
print("convert: fff", me);

me = mem.new("fdwbiscF", {1.1,2,3,4,5,6,7,8.88});
me = me.convert('iiii');
print("convert: iiii", me);
print("isValid", me.isValid())


