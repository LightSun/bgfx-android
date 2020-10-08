
local mem = require("memory")

local mat;
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("inverse", mat.inverse())

