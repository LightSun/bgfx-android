local function newlock()
    local lock = {
        acquire = function()
            print("acquire lock")
        end,
        release = function()
            print("release lock")
        end,
    }
    return lock
end

local function lockguard(lock)
    local wrap = {
        lock = lock
    }
    lock.acquire()
    return setmetatable(wrap, {__close = function(t, err)
        t.lock.release()
    end})
end

-- close: similar to java try-finally

local lock = newlock();
do
    for i = 1, 3 do
        local l <close> = lockguard(lock);
        print(i);
        error("esxpected error");
    end
end