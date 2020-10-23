
local mem = require("memory")

local mat;
-- 逆矩阵
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("inverse", mat.inverse())
mat = mem.newMatrix("i", {1, 2, 3}, {4, 5, 6}, {7, 8, 9})
assert(mat.inverse() == nil)
-- 行列式的值
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("determinant", mat.determinant())
--余子式值和矩阵
mat = mem.newMatrix("i", {1, 2, 10}, {-1, -3, 10}, {10, 10, 10})
print("remainderValue", mat.remainderValue(2, 2))
print("remainderMat", mat.remainderMat(2, 2))

--代数余子式 矩阵
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("algRemainderMat", mat.algRemainderMat()) --algebraicRemainderMat

--伴随矩阵
print("adjointMat", mat.adjointMat())