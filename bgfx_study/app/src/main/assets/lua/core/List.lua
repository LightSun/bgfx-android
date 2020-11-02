---heaven7
local it     = require("core.Iterator")
local coll   = require("core.Collection")
local CF     = require("core.CollectionFamily")
local utils  = require("core.utils.TableUtils")
local P      = require("core.utils.Predicates")
local T      = require("core.utils.Throwables")

local module = {};

--- create list with data-table
function module.new(list)
    -- check
    if (list and type(list) ~= "table") then
        error("param list must be table")
    end
    local self = coll.new(CF.COLLECTION_TYPE_LIST, "List", list)

    --- travel all elements
    function self.travel(func)
        for i = 1 , self.size() do
            local v = self.get(i - 1);
            local shouldBreak  = func(i, v);
            if (shouldBreak) then
                break
            end
        end
    end

    function self.get(index)
        return (self.getDataTable())[index + 1]
    end

    function self.remove(e)
        local result;
        local function traveller(index, value)
            if(value == e) then
                self.removeAt(index - 1)
                result = true;
                return true
            end
        end
        self.travel(traveller)
        return result
    end

    function self.removeAt(index)
        if (table.remove(self.getDataTable(), index + 1)) then
            self.addSize(-1)
            return true
        else
            return false;
        end
    end

    function self.set(index, val)
        if index >= self.size() then
            error("index out of range")
        end
        (self.getDataTable())[index + 1] = val
    end

    function self.add2(index, val)
        local len = self.size();
        table.insert(self.getDataTable(), index + 1, val)
        self.addSize(1)
        return self.size() > len
    end

    function self.insert(val, index)
        return self.add2(index, val);
    end

    function self.addMulti(...)
        local arg = { ... };
        for i = 1, #arg do
            self.add(arg[i])
        end
    end

    function self.add(val)
        local len = self.size();
        return self.add2(len, val)
    end

    -- ios
    function self.append(val)
        self.add(val)
    end

    function self.copy()
        local dt = {}
        for i = 1 , self.size() do
            dt[i] = self.get(i-1);
        end
        local a = module.new()
        a.setDataTable(dt)
        return a;
    end

    function self.iterator()
        -- here shouldn't use size() method
        local keys = {}
        for i = 1 , #self.getDataTable() do
            keys[i] = self.get(i-1);
        end

        local function listIterator()
            local ita   = it.new()
            --self.__index = self;
            ita.index   = -1;
            ita.size    = #keys;
            function ita.hasNext()
                return ita.index + 1 < ita.size;
            end

            function ita.next()
                ita.index = ita.index + 1
                return keys[ita.index + 1]
            end
            return ita;
        end
        return listIterator();
    end

    function self.indexOf(obj)
        for i = 1 , self.size() do
            if(self.get(i - 1) == obj) then
                return i - 1;
            end
        end
        return -1;
    end

    function self.lastIndexOf(obj)
        for i = self.size(), 1, -1 do
            if(self.get(i - 1) == obj) then
                return i - 1;
            end
        end
        return -1;
    end

    function self.getHead()
        return self.get(0);
    end

    function self.getTail()
        return self.get(self.size() - 1);
    end

    --- @param from: include
    --- @param to: exclude
    function self.subList(from, to, safe)
        assert(to > from)
        local size = self.size();
        if(safe and to > size) then
            to = size;
        end
        if (to > size) then
            error("wrong index of to = "..to)
        end

        local newList = module.new();
        for i = from + 1, to do
            local val = self.get(i - 1)
            -- print("subList. ", val)
            newList.add(val)
        end
        newList.recomputeSize()
        return newList;
    end

    function self.contains(e)
        return self.indexOf(e) >= 0
    end

    --- collection: must be list.
    function self.containsAll(collection)
        if (type(collection) ~= "table") then
            return nil
        end
        if(collection.getCollectionType and collection.getCollectionType() == CF.COLLECTION_TYPE_LIST) then
            if(self.size() ~= collection.size()) then
                return false;
            end
            local size = self.size();
            for i = 1 , size do
                if(self.get(i - 1) ~= collection.get(i-1)) then
                    return false;
                end
            end
        else
            return false;
        end
        return true;
    end

    function self.addAll(collection)
        T.checkTable(collection)
        local function func_list(tab)
            for i = 0, tab.size() - 1 do
                self.add(tab.get(i))
            end
            return true
        end
        local function func_set(tab)
            local ita = tab.iterator()
            while (ita.hasNext) do
                self.add(ita.next())
            end
            return true
        end
        local result = P.doWithCollection(collection, func_list, func_set)
        if (not result) then
            -- default like list
            local function traveller(key, value)
                self.add(value)
            end
            utils.travelTable(collection, traveller)
            result = true
        end
        return result
    end

    --- just convert to simple set
    function self.toSet()
        local set = {}
        local function traveller(index, value)
            set[value] = true
        end
        self.travel(traveller);
        return set
    end
    function self.sort(comp)
        table.sort(self.getDataTable(), comp);
        return self;
    end

    function self.equals(other)
        local m2
        local state, code = pcall(other.getCollectionType)
        if (state) then
            if (code == CF.COLLECTION_TYPE_SET) then
                return nil
            else
                if (code == CF.COLLECTION_TYPE_LIST) then
                    m2 = other;
                else
                    return nil;
                end
            end
        else
            m2 = module.new().setDataTable(other);
        end
        -- first check size
        -- print(self.size(), m2.size())
        if(self.size() ~= m2.size()) then
            return nil;
        end

        local result = true
        local function traveller(index, value)
            if (m2.get(index - 1) ~= value) then
                result = nil;
                return true;
            end
        end
        utils.travelTable(self, traveller);
        return result;
    end
    self.recomputeSize()

    -- meta methods
    local meta = {
        __eq = function(t1, t2)
            return t1.equals(t2);
        end
    , __add = function(t1, t2)
            if (t1.addAll(t2)) then
                return t1
            end
            return nil
        end
    , __tostring = function(tab)
            local msg = ""
            local function log(str)
                msg = str;
            end
            utils._logArray(tab, log)
            return msg
        end
    }
    setmetatable(self, meta)
    return self
end

--[[setmetatable(module, {
    __call = function (cls, t)
        return module.new(t)
    end,
})]]
return module