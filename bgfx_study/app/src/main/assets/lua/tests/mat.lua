
local mem = require("memory")

local mat;
-- 逆矩阵
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("inverse", mat.inverse())
-- mat.inverse 数据统一计算使用了double. 所以类型为‘F’
local r = mat.inverse() == mem.newMatrix("i", {3, 2}, {-1, -1});
print("inverse == ", r)
assert(not r)
assert(mat.inverse() == mem.newMatrix("F", {3, 2}, {-1, -1}))
assert(tostring(mat.inverse()) == '[[3,2],[-1,-1]]')

mat = mem.newMatrix("i", {1, 2, 3}, {4, 5, 6}, {7, 8, 9})
assert(mat.inverse() == nil)
-- 行列式的值
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("determinant", mat.determinant())
assert(mat.determinant() == -1)
--余子式值和矩阵
mat = mem.newMatrix("i", {1, 2, 10}, {-1, -3, 10}, {10, 10, 10})
print("remainderValue", mat.remainderValue(2, 2)) --10
assert(mat.remainderValue(2, 2) == -10)
print("remainderMat", mat.remainderMat(2, 2))
assert(tostring(mat.remainderMat(2, 2))== '[[1,2],[-1,-3]]')

--代数余子式 矩阵
mat = mem.newMatrix("i", {1, 2}, {-1, -3})
print("algRemainderMat", mat.algRemainderMat()) --algebraicRemainderMat
assert(tostring(mat.algRemainderMat()) == '[[-3,2],[2,-3]]')

--伴随矩阵
print("adjointMat", mat.adjointMat())
assert(tostring(mat.adjointMat()) == '[[-3,2],[2,-3]]')