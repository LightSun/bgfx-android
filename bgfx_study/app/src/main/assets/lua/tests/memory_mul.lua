
local mem = require("memory")

local m, c, mat;
print('--- test SkMemory --------');
m = mem.new("f", {1.5, 2.6, 3.235})
c = m * 2;
print("SkM_mul: number", c.getTypes(), c)

c = m * mem.new("d", {2, 3, 5})
print("SkM_mul: skm", c.getTypes(), #c, c)

c = m * mem.new("dfb", {2, 3, 5})
print("SkM_mul: skam", c.getTypes(), #c, c)


m = mem.new('f', {1, 2, 3})
mat = mem.newMat('dfb', { 10, 20, 30}, { 40, 50, 60})
c = m * mat;
print("SkM_mul: mat", c.getTypes(), #c, c)

print('--------- TEST SkAnymemory -------')
m = mem.new("dfb", {1, 2, 3})
c = m * 3;
print("SkAM_mul: number", c.getTypes(), c)

c = m * mem.new('f', {3.3, 5.5, 6.6})
print("SkAM_mul: skm", c.getTypes(), c)

c = m * mem.new("dfb", {2, 3, 5})
print("SkAM_mul: skam", c.getTypes(), #c, c)

mat = mem.newMat('dfb', { 10, 20, 30}, { 40, 50, 60})
c = m * mat;
print("SkAM_mul: mat", c.getTypes(), #c, c)

print('--------- TEST SkMatrix -------')
m = mem.newMat('d', { 1, 2, 3}, { 4, 5, 6})
c = m * 2.0;
print("SkMAT_mul: number", c.getTypes(), c)

--c = m * mem.new('f', {10, 20 , 30}); --
--print("SkMAT_mul: skm", c.getTypes(), c)
