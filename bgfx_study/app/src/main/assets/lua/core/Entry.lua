--- heaven7
local m = {}

function m.new(key, value)
    local k = key;
    local v = value;

    local self = {}
    function self.getType()
        return "Entry"
    end
    function self.getKey()
        return k;
    end

    function self.getValue()
        return v;
    end

    local meta = {
        __tostring = function(tab)
            return tostring(tab.getKey()).."_"..tostring(tab.getValue())
        end,
        __eq = function(t1, t2)
            if(t2.getType()~= "Entry") then
                return false;
            end
            return t1.getKey() == t2.getKey() and t1.getValue() == t2.getValue()
        end
    }
    setmetatable(self, meta)
    return self;
end

return m