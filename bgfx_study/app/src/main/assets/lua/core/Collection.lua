---heaven7

local it     = require("core.Iterator")
local CF     = require("core.CollectionFamily")
local utils  = require("core.utils.TableUtils")

local module = {}

function module.new(tt , typeName, data_tab, func_create, ...)
    local self = CF.new(tt , typeName, data_tab, func_create, ...);
    local size = 0;

    function self.size()
        return size
    end

    function self.isEmpty()
        return self.size() == 0;
    end

    function self.add(e)
        return nil;
    end
    function self.remove(e)
        return nil;
    end

    function self.contains(e)
        return nil;
    end

    function self.addAll(collection)
    end

    function self.removeAll(collection)
        local T      = require("core.Throwables")
        local P      = require("core.Predicates")

        T.checkTable(collection)
        local function func_list(tab)
            for i = 0 , tab.size() - 1 do
                self.remove(tab.get(i))
            end
            return true
        end
        local function func_set(tab)
            local ita = tab.iterator()
            while (ita.hasNext) do
                self.remove(ita.next())
            end
            return true
        end
        local result = P.doWithCollection(collection , func_list , func_set)
        if (not result) then
            -- default like map
            local function traveller(key , value)
                self.remove(value)
            end
            utils.travelTable(collection , traveller)
            result = true
        end
        return result
    end

    --- collection can be list. set.
    function self.containsAll(collection)
        return nil
    end

    function self.clear()
    end

    function self.iterator()
        return nil;
    end

    function self.copy()
        return nil
    end

    function self.toList()
        return self
    end

    function self.toSet()
        return self
    end

    function self.recomputeSize()
        local s   = 0
        local ita = self.iterator();
        if not ita or ita.getType() ~= it.TYPE then
            error("must be iterator.")
        end
        while ita.hasNext() do
            local val = ita.next();
            if (type(val) ~= "function") then
                s = s + 1;
            end
        end
        size = s;
       -- print("recomputeSize = ", s)
        return s;
    end
    -- protect methods ---

    function self.setSize(s)
        size = s
    end
    function self.addSize(delta)
        size = size + delta;
       -- print("after addSize, size = ", size)
    end

    return self;
end

return module;