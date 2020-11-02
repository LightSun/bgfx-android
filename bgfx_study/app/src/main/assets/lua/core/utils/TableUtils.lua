---heaven7
local En = require("core.Entry")
local CF = require("core.CollectionFamily")

local module                       = {}

module.MAP_TYPE_KEY                = 1;
module.MAP_TYPE_VALUE              = 2;
module.MAP_TYPE_PAIR               = 3;
module.MERGE_INCLUDE_METHODS       = 1;
module.MERGE_INCLUDE_OBJECT_MEMBER = 2;
module.MERGE_INCLUDE_TABLE         = 4;

function module.setMeta(obj, base)
    local meta = base or {}
    meta.__tostring = function(tab)
        return module.tostring(tab)
    end
    setmetatable(obj, meta)
end

--- travel tables all field. ignored functions
function module.travelTable(tab , func)
    if not tab then
        return nil
    end
    -- not table
    if (type(tab) ~= "table") then
        error("tab is not table")
    end

    if(tab.getCollectionType and type(tab.getCollectionType) == "function") then
        tab.travel(func)
    else
        local index;
        while true do
            -- travel all member.
            local key , val = next(tab , index);
            if (not val) then
                break
            end
            if (type(val) == "function") then
                -- ignored
            else
                local shouldBreak = func(key , val)
                if (shouldBreak) then
                    break
                end
            end
            index = key
        end
    end
end

--- travel tables with multi pairs. ignored functions
--- @param tab: the table
--- @param func: function to travel the pares. params: [table-Entry]
--- @param pairCount: how many pairs to pack
--- @param step: the step of pack. if nil means 1.
--- @param includeNoMatch: non nil means include not matched pair-count.
--- @param comp: the comparator of sort pairs before pack.
function module.travelMultiPairs(tab, func, pairCount, step, includeNoMatch, comp)
    local pairs = {};
    local function func_travel(index, value)
        table.insert(pairs, #pairs + 1, En.new(index, value))
    end
    module.travelTable(tab, func_travel)

    if(comp) then
        table.sort(pairs, comp)
    end

    -- travel
    if(not step) then
        step = 1;
    end
    -- end index
    local endIndex
    if(not includeNoMatch) then
        endIndex = #pairs - pairCount + 1
    else
        endIndex = #pairs
    end
    for i = 1 ,endIndex, step do
        -- add multi pairs to another list
        local list = {};
        for j = 1, pairCount do
            table.insert(list, #list + 1, pairs[i + j - 1])
        end
        if(func(list)) then
            break
        end
    end
end

---map table to another table.
---@param tab
---@param context
---@param func:the func with one or two func
---@param mapType: the map type. see module.MAP_TYPE_KEY, MAP_TYPE_VALUE, MAP_TYPE_PAIR
---@return :mapped table
function module.mapTable(tab , context , func , mapType)
    if not tab then
        return nil
    end
    -- not table
    if (type(tab) ~= "table") then
        error("tab is not table")
    end

    local resultMap               = {}
    -- prepare switch
    local switch                  = {}
    switch[module.MAP_TYPE_KEY]   = function(cx , key , value , f)
        local newKey = f(cx , key , value);
        if not newKey then
            error("new key can't be nil");
        end
        resultMap[newKey] = value;
    end
    switch[module.MAP_TYPE_VALUE] = function(cx , key , value , f)
        local newValue = f(cx , key , value);
        resultMap[key] = newValue;
    end
    switch[module.MAP_TYPE_PAIR]  = function(cx , key , value , f)
        if (type(f) ~= "table") then
            error("for map pair of key-value. f must be two functions. [1] is key-func, [2] is value-func");
        end
        local newKey = f[1](cx , key , value);
        if not newKey then
            error("new key can't be nil");
        end
        local newValue    = f[2](cx , key , value);
        resultMap[newKey] = newValue;
    end

    for key , value in pairs(tab) do
        switch[mapType](context , key , value , func)
    end
    return resultMap;
end

function module.copyTable(tab)
    local new_table = {}
    module.travelTable(tab ,
                       function(index , value)
                           new_table[index] = value;
                       end
    )
    return new_table
end

function module._logMap(tab , log_func)
    local str = "{ "
    local i   = 1;
    local function travel(key , value)
        if (type(value) == "function") then
            return
        end
        -- print("key-value: ",key, value)
        if (i ~= 1) then
            str = str .. ", "
        end
        i   = i + 1
        str = str .. key .. "=" .. tostring(value);
    end
    module.travelTable(tab , travel)
    str = str .. " }"
    log_func(str);
end

function module.print(tab)
    print(module.tostring(tab))
end

function module.tostring(tab)
    local str = "{"
    local init;
    local function travel(key , value)
        if(not init )then
            init = true;
        else
            str = str..", "
        end
        if(type(value) == "function") then
            -- ignore
        else if(type(value) == "table") then
            str = str..tostring(key).." = "..module.tostring(value);
        else
            str = str..tostring(key).." = "..tostring(value);
        end
        end
    end
    module.travelTable(tab , travel)
    str = str.."}";
    return str;
end

function module.logMap(tab)
    module._logMap(tab , print)
end

-- lua 不支持方法重载
function module._logArray(arr , log_func)
    local str
    local function log(index, value)
        if(not str) then
            str = "{ "
        else
            str = str..", "
        end
        str = str .. tostring(value);
    end
    module.travelTable(arr, log)

    if(not str) then
        log_func("{}");
        return
    end
    str = str .. " }"
    log_func(str);
end

function module.mapArray(arr , context , func)
    if not arr then
        return nil
    end
    local newArr = {}
    if arr then
        for i = 1 , #arr do
            newArr[i] = func(context , arr[i])
        end
    end
    return newArr;
end

function module.logArray(arr)
    module._logArray(arr , print)
end

function module.logSet(set)
    module._logSet(set, print)
end

function module._logSet(set, func)
    local str
    local function log(index, value)
        if(not str) then
            str = "{ "
        else
            str = str..", "
        end
        str = str .. tostring(index);
    end
    module.travelTable(set, log)
    str = str .. " }"
    func(str);
end

--- merge tables like map ---
function module.mergeMap(receiver , ...)
    local result = receiver or {}
    local tabs   = { ... }
    if not tabs then
        return result
    end

    local function merge(index , value)
        result[index] = value;
    end

    for i = 1 , #tabs do
        if tabs[i] then
            module.travelTable(tabs[i] , merge)
        end
    end
    return result
end

function module.mergeArray(receiver , ...)
    local result = receiver or {}
    local tabs   = { ... }
    if not tabs then
        return result
    end

    local function merge(index , value)
        table.insert(result , value);
    end

    for i = 1 , #tabs do
        if tabs[i] then
            module.travelTable(tabs[i] , merge)
        end
    end
    return result
end

--- judge the two table is equals or not
--- @param tab1: table 1
--- @param tab2: table 2
---@return: true if equals. nil other wise
function module.equalsList(tab1 , tab2)
    if (not tab1) then
        if (not tab2) then
            return true
        else
            return nil
        end
    end
    if (not tab2) then
        return nil
    end

    if (type(tab1) ~= "table") then
        if (type(tab2) == "table") then
            return nil
        else
            return tab1 == tab2
        end
    end
    for k , v in pairs(tab1) do
        local t = type(v);
        if (t == "table") then
            if (not tab2[k] or type(tab2[k]) ~= "table" or not module.equalsList(v , tab2[k])) then
                return nil
            end
        else
            if (t == "function") then
                -- ignore this.
            else
                if (t == "thread" or t == "userdata") then
                    error("can't compare thread and userdata")
                else
                    if (not tab2[k] or v ~= tab2[k]) then
                        return nil
                    end
                end
            end
        end

    end
    return true;

end

--- whether the tab1 contains all tab2's elements
function module.containsAllList(tab1 , tab2)
    if (not tab1 or not tab2) then
        return nil
    end

    for k , v in pairs(tab2) do
        local t = type(v);
        if (t == "table") then
            if (not tab1[k] or type(tab1[k]) ~= "table" or not module.containsAllList(v , tab1[k])) then
                return nil
            end
        else
            if (t == "function") then
                -- ignore this.
            else
                if (t == "thread" or t == "userdata") then
                    error("can't compare thread and userdata")
                else
                    if (not tab1[k] or v ~= tab1[k]) then
                        return nil
                    end
                end
            end
        end
    end

    return true
end

--- get a table value from variable params which as a table.
---@param index: start from 0.
function module.getAt(index , ...)
    local p = { ... }
    return p[index + 1];
end

return module