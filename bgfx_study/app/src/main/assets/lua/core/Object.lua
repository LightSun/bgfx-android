---heaven7
local m      = {}

local function getAt(index , ...)
    local p = { ... }
    return p[index + 1];
end

---create super object .
---@param typeName: the type name of current object
---@param func_create: function to create object. can be nil
---@param ...: the params to create object
function m.new(typeName , func_create , ...)
    assert(type(typeName) == "string")
    local self;
    if (not func_create or type(func_create) ~= "function") then
        self = {}
    else
        self = func_create(...) or {};
    end

    return m.wrap(self, typeName);
end

--- wrap an object with methods
function m.wrap(obj, name)
    assert(obj)
    local self = obj;
    local __type = name;
    function self.getType()
        return __type
    end
    function self.equals(other)
        return nil
    end

    -- self.__eq  =
    -- self.__add +
    -- self.__sub -
    -- self.__mul *
    -- self.__div /
    -- self.__unm  相反数
    -- __mod  %
    -- __pow ^
    --__concat
    -- __lt  <
    -- le   <=

    return self;
end

function m.new1(typeName ,p)
    local function create(...)
        return getAt(0, ...)
    end
    return m.new(typeName, create, p)
end

return m

