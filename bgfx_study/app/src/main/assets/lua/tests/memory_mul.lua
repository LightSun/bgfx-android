
local mem = require("memory")

--- test SkMemory --------
local m = mem.new("f", {1.5, 2.6, 3.235})
local c = m * 2;
print("SkM_mul: number", c.getTypes(), c)

c = m * mem.new("d", {2, 3, 5})
print("SkM_mul: skm", c.getTypes(), #c, c)

c = m * mem.new("dfb", {2, 3, 5})
print("SkM_mul: skam", c.getTypes(), #c, c)

m = mem.new('f', {1, 2, 3})
local mat = mem.newMatrix('dfb', {10, 20, 30}, {40, 50, 60})
c = m * mat;
print("SkM_mul: mat", c.getTypes(), #c, c)
