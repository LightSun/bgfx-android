---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/10/26 0026 下午 6:14
---
local mem = require("hmem_lua")
print("start test mem.computeTypes", mem);
local function assertB(c1, c2, ex)
    print(c1, c2, ' -> ', ex);
    assert(mem.test_comp(c1, c2), ex);
end

-- f,d,w,b, i, s, c, F
print("base: ", 'b')
assertB('b', 'b', 'b');
assertB('b', 'w', 'w');
assertB('b', 'd', 'd');
assertB('b', 'f', 'f');

assertB('b', 'c', 's');
assertB('b', 's', 's');
assertB('b', 'i', 'i');
assertB('b', 'F', 'F');

print("base: ", 'w')
assertB('w', 'b', 'w');
assertB('w', 'w', 'w');
assertB('w', 'd', 'd');
assertB('w', 'f', 'f');

assertB('w', 'c', 'i');
assertB('w', 's', 'i');
assertB('w', 'i', 'i');
assertB('w', 'F', 'F');

print("base: ", 'd')
assertB('d', 'b', 'd');
assertB('d', 'w', 'd');
assertB('d', 'd', 'd');
assertB('d', 'f', 'F');

assertB('d', 'c', 'F');
assertB('d', 's', 'F');
assertB('d', 'i' , 'F');
assertB('d', 'F', 'F');

print("base: ", 'f')
assertB('f', 'b', 'f');
assertB('f', 'w', 'f');
assertB('f', 'd', 'F');
assertB('f', 'f', 'f');

assertB('f', 'c', 'f');
assertB('f', 's', 'f');
assertB('f', 'i', 'F');
assertB('f', 'F', 'F');

print("base: ", 'c')
assertB('c', 'b', 's');
assertB('c', 'w', 'i');
assertB('c', 'd', 'F');
assertB('c', 'f', 'f');

assertB('c', 'c', 'c');
assertB('c', 's', 's');
assertB('c', 'i', 'i');
assertB('c', 'F', 'F');

print("base: ", 's')
assertB('s', 'b', 's');
assertB('s', 'w', 'i');
assertB('s', 'd', 'F');
assertB('s', 'f', 'f');

assertB('s', 'c', 's');
assertB('s', 's', 's');
assertB('s', 'i', 'i');
assertB('s', 'F', 'F');

print("base: ", 'i')
assertB('i', 'b', 'i');
assertB('i', 'w', 'i');
assertB('i', 'd', 'F');
assertB('i', 'f', 'F');

assertB('i', 'c', 'i');
assertB('i', 's', 'i');
assertB('i', 'i', 'i');
assertB('i', 'F', 'F');

print("base: ", 'F')
assertB('F', 'b', 'F');
assertB('F', 'w', 'F');
assertB('F', 'd', 'F');
assertB('F', 'f', 'F');

assertB('F', 'c', 'F');
assertB('F', 's', 'F');
assertB('F', 'i', 'F');
assertB('F', 'F', 'F');