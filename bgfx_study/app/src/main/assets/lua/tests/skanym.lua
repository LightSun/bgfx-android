---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/10/10 0010 下午 5:38
---
local mem = require("memory")
local m, r;
m = mem.new('iii', { 1, 2, 3 }, { -4, -5, -6 })

print("---------- test AnyMemory-> equals ----------")
assert(m == mem.new('iii', { 1, 2, 3 }, { -4, -5, -6 }))
assert(not (m == mem.new('i', { 1, 2, 3 }, { -4, -5, -6 })))

print("------- start test AnyMemory -> extract ------ ")
r = m.extract(2, 5);
print("extract", r)
assert(#r == 3)
assert(r[0] == 3)
assert(r[1] == -4)
assert(r[2] == -5)

print("------- start test AnyMemory -> kickOut ------ ")
r = m.kickOut(2);
print("kickOut", r)
assert(#r == 5)
assert(r[0] == 1)
assert(r[1] == 2)
assert(r[2] == -4)
assert(r[3] == -5)
assert(r[4] == -6)

-- reshape, concat, flip, diag like matlib
m = mem.new('id', { 1, 2, 3, 4 })
print("------ start test AnyMemory-> reshape -------")
--default reshape type is merged: 'if' merged to 'F' as double. reshape count is 6
assert(m.reshape(6), mem.new('F', { 1, 2, 3, 4, 0, 0 }));
-- assigned result type is 'i' as int.
assert(m.reshape(6, 'i'), mem.new('i', { 1, 2, 3, 4, 0, 0 }));
assert(m.reshape(6, 'i', 10), mem.new('i', { 1, 2, 3, 4, 10, 10 }));
-- row count = 3, column count = 2, type is 'd', default value 0
assert(m.reshape(3, 2, 'd') == mem.newMat('d',
        {1, 2},
        {3, 4},
        {0, 0}
))

print("------- start test AnyMemory -> concat ------ (mem, count, type, defVal)")
r = m.concat(mem.new('i', { 5, 6 }));
assert(r == mem.new('F', { 1, 2, 3, 4, 5, 6 }))
-- result count = 8,  default val 0
r = m.concat(mem.new('i', { 5, 6 }), 8);
assert(r == mem.new('F', { 1, 2, 3, 4, 5, 6, 0, 0}))

-- result count = 8,  default val 0, assigned type to 'i' as int.
r = m.concat(mem.new('i', { 5, 6 }), 8, 'i');
assert(r == mem.new('i', { 1, 2, 3, 4, 5, 6, 0, 0}))

-- result count = 8,  default val 10, assigned type to 'i' as int.
r = m.concat(mem.new('i', { 5, 6 }), 8, 'i', 10);
assert(r == mem.new('i', { 1, 2, 3, 4, 5, 6, 10, 10}))

print("------- start test AnyMemory -> flip ------ ")
r = m.flip();
print("flip", r)
assert(r == mem.new('F', {4, 3, 2, 1}))

print("------- start test AnyMemory -> diag ------ ")
--default k = 0.
r = m.diag();
assert(r == mem.newMat('F',
        {1, 0, 0, 0},
        {0, 2, 0, 0},
        {0, 0, 3, 0},
        {0, 0, 0, 4}
))
--default k = 0. default value -100
r = m.diag(0, -100);
assert(r == mem.newMat('F',
        {1, -100, -100, -100},
        {-100, 2, -100, -100},
        {-100, -100, 3, -100},
        {-100, -100, -100, 4}
))
--default k = -1. default value -100
r = m.diag(-1, -100);
assert(r == mem.newMat('F',
        {-100, -100, -100, -100, -100},
        {1, -100, -100, -100, -100},
        {-100, 2, -100, -100, -100},
        {-100, -100, 3, -100, -100},
        {-100, -100, -100, 4, -100}
))

--default k = 1. default value -100
r = m.diag(1, -100);
assert(r == mem.newMat('F',
        {-100, 1, -100, -100, -100},
        {-100, -100, 2, -100, -100},
        {-100, -100, -100, 3, -100},
        {-100, -100, -100, -100, 4},
        {-100, -100, -100, -100, -100}
))
--k = 1. type is int ,default value -100
r = m.diag(1, 'i', -100);
assert(r == mem.newMat('i',
        {-100, 1, -100, -100, -100},
        {-100, -100, 2, -100, -100},
        {-100, -100, -100, 3, -100},
        {-100, -100, -100, -100, 4},
        {-100, -100, -100, -100, -100}
))

print("==================== start test mergeUnit =================")
m = mem.new('dfb', {1, 2, 3});
r = m.mergeUnit('d', 3, function (tab)
    local sum = 0;
    for _, v in ipairs(tab) do
        sum = sum + v;
    end
    return sum;
end);
print("mergeUnit", r)
assert(#r == 1)
assert(r[0] == 6)
assert(r.getTypes() == 'd')
print("===== test mergeUnit ok ====")

print("==================== start test splitUnit =================")
r = r.splitUnit('i', 3, function (val, count)
    return {1, 2, 3}
end);
assert(r == mem.new("i", {1, 2, 3}))

print("===== test splitUnit ok ====")