
local lib = require "int64"

local int64 = lib.new
--print(0xff000000)

--  int64: 0x101005000000010	int64: 0x101005000000020
print(int64(72339412612022291), int64(0x10100500000001F)) -- state: curbes.cpp, curbes.lua

print("int64(0x0100000000000000)", int64(0x0100000000000000))
print(lib.tostring(int64 "\1\2\3\4\5\6\7\8"))

local a = 1 + int64(1)
local b = int64 "\16" + int64("9",10)
print(lib.tostring(a,10), lib.tostring(b,2))
print("+", a+b)
print("-", lib.tostring(a-b,10))
print("*", a*b)
print("/", a/b)
print("%", a%b)
print("^", a^b)
print("==", a == b)
print(">", a > b)
print("#", #a)

--I/Lua_Print: int64: 0x807060504030201
--I/Lua_Print: 2	0000000000000000000000000000000000000000000000000000000000011001
--I/Lua_Print: +	int64: 0x1B
--I/Lua_Print: -	-23
--I/Lua_Print: *	int64: 0x32
--I/Lua_Print: /	int64: 0x0
--I/Lua_Print: %	int64: 0x2
--I/Lua_Print: ^	int64: 0x2000000
--I/Lua_Print: ==	false
--I/Lua_Print: >	false
--I/Lua_Print: #	2.0
--I/Luaer: called [ loadLua() ]: state = 0, null