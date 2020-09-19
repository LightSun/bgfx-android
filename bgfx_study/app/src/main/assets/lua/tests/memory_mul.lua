
local mem = require("memory")

--- test transpose
local m = mem.new("f", {1.5, 2.6, 3.235})
local c = m * 2;
print("SkM_mul", c)

c = m * mem.new("d", {2, 3, 5})