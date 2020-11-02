---heaven7
local obj    = require("core.Object")

--- heaven7
local m = {}
m.COLLECTION_TYPE_LIST = 1
m.COLLECTION_TYPE_SET  = 2
m.COLLECTION_TYPE_MAP  = 3

function m.new(c_type, typeName, data_tab, func_create, ...)
    if(data_tab ) then
        assert(type(data_tab) == "table")
    end
    local self;
    if(data_tab and data_tab.getCollectionType) then
        self = data_tab;
    else
        self = obj.new(typeName , func_create , ...);
        self._DT_TABLE = data_tab or {}
    end

    local t = c_type;

    function self.size()
        return 0;
    end

    function self.recomputeSize()
    end

    function self.getCollectionType()
        return t
    end

    function self.getKey(key, value)
        return key;
    end
    function self.getValue(key, value)
        return value;
    end

    function self.copy()

    end
    --- currently only List support sort
    function self.sort(comp)

    end
    function self.getDataTable()
        if(not self._DT_TABLE) then
            self._DT_TABLE = {}
        end
       return self._DT_TABLE
    end
    function self.setDataTable(tab, ignoreSize)
        assert(tab)
        self._DT_TABLE = tab;
        if(not ignoreSize) then
            self.recomputeSize()
        end
        return self;
    end
    function self.recomputeSize()
        error("unsuport for this type = "..t);
    end
    function self.travel(func)
        error("unsuport for this type = "..t);
    end
    return self;
end

return m