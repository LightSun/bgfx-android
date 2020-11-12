--- heaven7
local m = {}

local function isTargetType(obj, target)
    if (type(obj) == target) then
        return true
    end
    return nil;
end

function m.isTable(tab)
    return isTargetType(tab, "table");
end

function m.isList(tab)
    if(type(tab)~="table")then
        return nil;
    end
    local CF = require("core.CollectionFamily")

    local state, result = pcall(tab.getCollectionType)
    if(not state or result ~=  CF.COLLECTION_TYPE_LIST) then
        return nil;
    end
    return true
end

function m.isMap(tab)
    if(type(tab)~="table")then
        return nil;
    end
    local CF = require("core.CollectionFamily")

    local state, result = pcall(tab.getCollectionType)
    if(not state or result ~=  CF.COLLECTION_TYPE_MAP) then
        return nil;
    end
    return true
end

--- check if is collection member.
---@param tab: the table
---@param msg: the msg
---@param ext: the List which used to contains extra collection types.
function m.isCollection(tab, ext)
    local state, result = pcall(tab.getCollectionType)
    if (not state) then
        return nil
    else
        local CF = require("core.CollectionFamily")

        local cs;
        if (ext) then
            cs = ext.contains
        else
            cs = nil
        end
        if (result ~= CF.COLLECTION_TYPE_LIST and result ~= CF.COLLECTION_TYPE_SET
                and result ~= CF.COLLECTION_TYPE_MAP and (not cs or not cs(result))) then
            return nil
        end
    end

    return true
end

--- do if tab is collection. else return nil
---@param tab: table
---@param func_list: function list. fun(t:table): any
---@param func_set: function set. fun(t:table): any
---@param fun_map: function map. fun(t:table): any
---@param ext: List. which contains all extra collection types
---@param func_other: function. func(t:table, num:number): any
---@return nil or function result
function m.doWithCollection(tab, func_list, func_set, fun_map, ext, func_other)
    assert(tab)
    local state, result = pcall(tab.getCollectionType)
    if (not state) then
        return nil
    else
        local CF = require("core.CollectionFamily")
        if (result == CF.COLLECTION_TYPE_LIST and m.isFunction(func_list)) then
            return func_list(tab)
        end
        if (result == CF.COLLECTION_TYPE_SET and m.isFunction(func_set)) then
            return func_set(tab)
        end
        if (result == CF.COLLECTION_TYPE_MAP and m.isFunction(fun_map)) then
            return fun_map(tab)
        end

        local cs;
        if (ext) then
            cs = ext.contains
        else
            cs = nil
        end
        if ((not cs or not cs(result))) then
            return nil
        end
    end
    return func_other(tab, result)
end

function m.isFunction(func)
    return isTargetType(func, "function");
end

function m.isString(str)
    return isTargetType(str, "string");
end

function m.isNumber(num)
    return isTargetType(num, "number");
end

function m.isInteger(num)
    return math.type(num) == "integer"
end

function m.isFloat(num)
    return math.type(num) == "float"
end

function m.isTrue(val)
    return val and val == true
end

function m.isNil(obj)
    return not obj
end

function m.isBool(obj)
    return type(obj) == "boolean"
end

return m