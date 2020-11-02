--- heaven7
local utils  = require("core.utils.TableUtils")
local it     = require("core.Iterator")
local coll   = require("core.Collection")
local CF   = require("core.CollectionFamily")

local module = {};

local function listToSet(list)
    if(not list) then
        return nil;
    end
    local dataTab;
    local function create(...)
        local p = { ... }
        if (not p[1]) then
            return nil
        end
        local set = {}
        local function traveller(index, value)
            if (type(value) ~= "function") then
                set[value] = true
            end
        end
        utils.travelTable(p[1] , traveller)
        return set;
    end
    dataTab = create(list)
    return dataTab
end

function module.newFromList(set)
    return module.new(listToSet(set))
end

function module.new(setData)
    if (setData and type(setData) ~= "table") then
        error("must be table set")
    end

    local self = coll.new(CF.COLLECTION_TYPE_SET, "Set", setData)
    ---- methods ---------------
    ---
    function self.travel(func)
        for k, v in pairs(self.getDataTable()) do
            if func(k, v) then
                return
            end
        end
    end
    function self.add(e)
        local old = self.getDataTable()[e]
        self.getDataTable()[e]   = true
        -- pre not exist
        if (not old) then
            self.addSize(1)
            return true
        end
        return nil
    end

    function self.remove(e)
        local old = self.getDataTable()[e]
        self.getDataTable()[e]   = nil
        -- pre exist
        if (old) then
            self.addSize(-1)
            return true
        end
        return nil
    end

    function self.contains(e)
        return self.getDataTable()[e] ~= nil
    end

    function self.addAll(collection)
        if (not collection or type(collection) ~= "table") then
            return nil
        end
        local state , code = pcall(collection.getType)
        local handled
        if (state) then
            if (code == "List") then
                local list = collection
                for i = 0 , list.size() - 1 do
                    self.add(list.get(i))
                end
                handled = true;
            else
                if (code == "Set") then
                    local ita = collection.iterator()
                    while (ita.hasNext()) do
                        self.add(ita.next());
                    end
                    handled = true;
                end
            end
        else
            -- default.
        end
        -- as array
        if (not handled) then
            -- collection can be normal array
            local function traveller(index , value)
                self.getDataTable()[value] = true
            end
            utils.travelTable(collection , traveller)
            self.recomputeSize()
            handled = true;
        end

        return handled;
    end

    function self.containsAll(collection)
        if (not collection or type(collection) ~= "table") then
            return nil
        end
        local state , code = pcall(collection.getType)
        local handled
        if (state) then
            if (code == "List") then
                local list = collection
                for i = 0 , list.size() - 1 do
                    local v = list.get(i);
                    if (self.getDataTable()[v] == nil) then
                        return nil
                    end
                end
                handled = true;
            else
                if (code == "Set") then
                    local ita = collection.iterator()
                    while (ita.hasNext()) do
                        local v = ita.next();
                        if (self.getDataTable()[v] == nil) then
                            return nil
                        end
                    end
                    handled = true;
                end
            end
        else
            -- default.
        end

        if (not handled) then
            for i = 1 , #collection do
                local v = collection[i];
                if (type(v) ~= "function") then
                    if (self.getDataTable()[v] == nil) then
                        return nil
                    end
                end
            end
            handled = true;
        end

        return handled;
    end

    function self.clear()
        self.setDataTable({}, true)
        self.setSize(0)
    end

    function self.toList()
        local keys = {}
        -- for set.
        local function traveller(index , value)
            table.insert(keys , #keys + 1 , index)
        end
        self.travel(traveller)
        return keys;
    end

    function self.iterator()
        local keys = self.toList()

        local function listIterator()
            local self   = it.new()
            self.__index = self;
            self.index   = -1;
            self.size    = #keys;
            function self.hasNext()
                return self.index + 1 < self.size;
            end

            function self.next()
                self.index = self.index + 1
                return keys[self.index + 1]
            end
            return self;
        end
        return listIterator();
    end

    function self.getValue(key, value)
        return key;
    end

    function self.copy()
        local dt = {}
        self.travel(function (k, v)
            dt[k] = v;
        end)
        return module.new(dt)
    end

    function self.equals(other)
        if(not other or type(other) ~= "table") then
            return nil
        end
        -- cast to set
        local oth;
        local state, code = pcall(other.getType)
        if(state) then
            if(code == "List") then
                oth = module.newFromList(other)
            else if(code == "Set") then
                oth = other
            else
                error("throw new unsupport type for set.equals. target table is "..tostring(other))
            end
            end
        else
            oth = module.newFromList(other)
        end

        -- start travel
        local equ = true;
        local function traveller(index, value)
            if(not oth.contains(index)) then
                equ = nil;
                return true;
            end
            return nil
        end
        utils.travelTable(self, traveller)
        return equ;
    end

    self.recomputeSize()

    -- meta
    local meta = {
        __eq = function(t1, t2)
            return t1.equals(t2);
        end
    ,__add = function(t1, t2)
            if(t1.addAll(t2)) then
                return t1
            end
            return nil
        end
    , __tostring = function(tab)
            local msg = ""
            local function log(str)
                msg = str;
            end
            utils._logSet(tab, log)
            return msg
        end
    }
    setmetatable(self, meta)
    return self;
end

return module;

