--- heaven7
local m = {}

function m.checkTable(tab, msg)
    if(not msg) then
        msg = tostring(tab).." must be table."
    end
    if(type(tab) ~= "table") then
        error(msg)
    end
end

--- check the table which can be nil
function m.checkTableOrNil(tab, msg)
    if(tab) then
        m.checkTable(tab, msg)
    end
end

function m.checkList(tab, msg)
    if(not msg) then
        msg = tostring(tab).." must be List."
    end
    m.checkTable(tab)
    local CF = require("core.CollectionFamily")

    local state, result = pcall(tab.getCollectionType)
    if(not state or result ~=  CF.COLLECTION_TYPE_LIST) then
        error(msg)
    end
end
--- check table must be collection member.
---@param tab: the table
---@param msg: the msg
---@param ext: the List which used to contains extra collection types.
function m.checkCollection(tab, msg, ext)
    if(not msg) then
        msg = tostring(tab).." must be one of collection-family."
    end
    m.checkTable(tab, msg)
    local state, result = pcall(tab.getCollectionType)
    if(not state) then
        error(msg)
    else
        local CF = require("core.CollectionFamily")

        local cs;
        if(ext) then
            cs = ext.contains
        else
            cs = nil
        end
        if(result ~= CF.COLLECTION_TYPE_LIST and result ~= CF.COLLECTION_TYPE_SET
                and result ~= CF.COLLECTION_TYPE_MAP and (not cs or not cs(result))) then
            error(msg)
        end
    end
end

--- check table must be collection member.
---@param tab: the table
---@param msg: the msg
---@param ext: the List which used to contains extra collection types.
function m.checkCollectionOrNil(tab, msg, ext)
    if(tab) then
        m.checkCollection(tab, msg, ext)
    end
end

function m.checkString(str, msg)
    if(not msg) then
        msg = tostring(str).." must be string."
    end
    if(type(str) ~= "string") then
        error(msg)
    end
end

function m.checkStringOrNil(str, msg)
    if(str) then
        m.checkString(str, msg)
    end
end

function m.checkNumber(num, msg)
    if(not msg) then
        msg = tostring(num).."must be number."
    end
    if(type(num) ~= "number") then
        error(msg)
    end
end

function m.checkNumberOrNil(num, msg)
    if(num) then
        m.checkNumber(num, msg)
    end
end

function m.checkNonNil(obj, msg)
    if(not msg) then
        msg = tostring(obj).." must not be nil."
    end
    if(not obj) then
        error(msg)
    end
end

function m.checkFunction(func, msg)
    if(not msg) then
        msg = tostring(func).." must be function."
    end
    if(type(func) ~= "function") then
        error(msg)
    end
end

function m.checkFunctionOrNil(func, msg)
    if(func) then
        m.checkFunction(func, msg)
    end
end

--- must be non - negative
function m.checkNonNegative(val, msg)
    if(not msg) then
        msg = tostring(val).." must be >= 0."
    end
    if(val < 0) then
        error(msg)
    end
end

--- must be negative
function m.checkNegative(val, msg)
    if(not msg) then
        msg = tostring(val).." must be < 0."
    end
    if(val >= 0) then
        error(msg)
    end
end

--- must be negative
function m.checkPositive(val, msg)
    if(not msg) then
        msg = tostring(val).." must be > 0."
    end
    if(val <= 0) then
        error(msg)
    end
end

return m