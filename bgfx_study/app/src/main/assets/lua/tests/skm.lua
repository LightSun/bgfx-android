---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/10/10 0010 下午 5:38
---
local mem = require("memory")
local m, r;
m = mem.new('i', {1, 2, 3}, {-4, -5, -6})
print("---------- test equals ----------")
assert(m == mem.new('i', {1, 2, 3}, {-4, -5, -6}))
assert(not (m == mem.new('s', {1, 2, 3}, {-4, -5, -6})))

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

-- reshape, concat, flip, diag
print("------- start test SkMemory -> reshape (rowC, [colC, type, def_val]) ------ ")
m = mem.new('i', {1, 2, 3});
print("reshape", m.reshape(4))
assert(m.reshape(4) == mem.new('i', {1, 2, 3, 0}));
assert(m.reshape(4, 'f') == mem.new('f', {1, 2, 3, 0}));
assert(m.reshape(4, 'd', 10) == mem.new('d', {1, 2, 3, 10}));
-- row count = 3, column count = 2, default value = 10.
assert(m.reshape(3, 2, 'd', 10) == mem.newMat('d',
        {1, 2},
        {3, 10},
        {10, 10}
));

print("------- start test SkMemory -> concat ------ (mem, [count, type, defVal])")
r = m.concat(mem.new('i', {4, 5, 6}));
print("concat", r)
assert(r == mem.new('i', {1, 2, 3, 4, 5, 6}))

r = m.concat(mem.new('i', {4, 5, 6}), 8);
assert(r == mem.new('i', {1, 2, 3, 4, 5, 6, 0, 0}))

r = m.concat(mem.new('f', {4, 5, 6}), 8);
assert(r == mem.new('f', {1, 2, 3, 4, 5, 6, 0, 0}))

r = m.concat(mem.new('f', {4, 5, 6}), 8, 'f');
assert(r == mem.new('f', {1, 2, 3, 4, 5, 6, 0, 0}))

r = m.concat(mem.new('f', {4, 5, 6}), 8, 'd', 10);
assert(r == mem.new('d', {1, 2, 3, 4, 5, 6, 10, 10}))

print("------- start test SkMemory -> flip ------ ")
assert(m.flip() == mem.new('i', {3, 2, 1}))

print("------- start test SkMemory -> diag ------ ")
-- default k = 0, default value = 0
r = m.diag();
print("diag =",r)
assert(r == mem.newMat('i',
        {1, 0, 0},
        {0, 2, 0},
        {0, 0, 3}
))
-- with k = -1.
r = m.diag(-1);
assert(r == mem.newMat('i',
        {0, 0, 0, 0},
        {1, 0, 0, 0},
        {0, 2, 0, 0},
        {0, 0, 3, 0}
))
-- with k = -1, and default value is 100.
r = m.diag(-1, 100);
assert(r == mem.newMat('i',
        {100, 100, 100, 100},
        {1, 100, 100, 100},
        {100, 2, 100, 100},
        {100, 100, 3, 100}
))

-- with k = 1.
r = m.diag(1);
print("diag(1) =", r)
assert(r == mem.newMat('i',
        {0, 1, 0, 0},
        {0, 0, 2, 0},
        {0, 0, 0, 3},
        {0, 0, 0, 0}
))

-- with k = 1. default value -100
r = m.diag(1, -100);
assert(r == mem.newMat('i',
        {-100, 1, -100, -100},
        {-100, -100, 2, -100},
        {-100, -100, -100, 3},
        {-100, -100, -100, -100}
))

-- with k = 1. type is 's' as short, default value -100
r = m.diag(1, 's', -100);
assert(r == mem.newMat('s',
        {-100, 1, -100, -100},
        {-100, -100, 2, -100},
        {-100, -100, -100, 3},
        {-100, -100, -100, -100}
))