
local mem = require("memory")

local mat;
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("inverse", mat.inverse())
mat = mem.newMatrix("i", {1, 2, 3}, {4, 5, 6}, {7, 8, 9})
assert(mat.inverse() == nil)
