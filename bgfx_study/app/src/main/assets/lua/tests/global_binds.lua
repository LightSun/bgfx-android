

--- add the sourcefiles as the sub file of dir.
function utils_addCppSrc(dir,...)
	local t = {...}
	local len1 = #(t)
	local out = {}
	for i=1, len1 do
		table.insert(out, dir.."/"..t[i])
	end
	return out;
end

-----------------------------------------
globalBinds = {}
globalBinds.funcs = {}
globalBinds.globalLock = {};
globalBinds.parentObj = nil;
globalBinds.parentLock = {}

local function lock(obj) -- may add timeout
	--todo impl of lock obj

end

local function unlock(obj)
end

-- used to 'one type-one object' or one-multi
-- call this can't in multi thread
local function registerFunc(g_desc)
	print("registerFunc: ", g_desc.name)
	local name = g_desc.name;
	local key = g_desc.getChainKey()
	-- attach to global function
	local func = globalBinds.funcs[name];

	if(not func) then
		_G[name] = function(args)
			print("-- registerFunc run: ", key)
			lock(globalBinds.parentLock)
			local parent = globalBinds.parentObj;
			unlock(globalBinds.parentLock)

			local t;
			if(parent) then
				t = globalBinds.funcs[key]
			else
				t = globalBinds.funcs[name]
			end

			if(t) then
				local obj = t.inflate(args)
				lock(globalBinds.parentLock)
				globalBinds.parentObj = obj;
				unlock(globalBinds.parentLock)
			else
				error("no impl for func: "..name)
			end
		end
	end
	-- add desc
	print("key: ", g_desc.getChainKey())
	globalBinds.funcs[g_desc.getChainKey()] = g_desc
	print("registerFunc end: ", g_desc.name)
end

function newApiDesc(desc)
	if(not desc) then
		desc = {}
	end
	--print("newApiDesc: ", desc.name);

	local chainKey;
	function desc.getChainKey()
		if(not chainKey) then
			local key = desc.name;
			local parent = desc.parentDesc;
			while (parent) do
				key = parent.name.."_"..key;
				parent = parent.parentDesc;
			end
			chainKey = key;
		end
		return chainKey;
	end

	local scope;
	function desc.getScope()
		if(not scope) then
			local parent = desc.parentDesc;
			if(parent ) then
				scope = parent.name;
			end
		end
		return scope;
	end

	function desc.getRootObj()
		local root = desc.infObj;
		local parent = desc.parentDesc;
		while (parent) do
			root = parent.infObj;
			parent = desc.parentDesc;
		end
		return root;
	end
	function desc.inflate(args)
		lock(desc.lockObj);
		local obj = desc.inflator(desc, args)
		unlock(desc.lockObj);
		if(not obj) then
			if(not desc.infObj) then
				desc.infObj = {}
			end
			obj = desc.infObj;
		end
		return obj
	end

	function desc.addChild(child)
		child.parentDesc = desc;
		if(not desc.subDescs) then
			desc.subDescs = {}
		end
		table.insert(desc.subDescs, child)
	end

	return desc;
end

-- untis_configBuilder({})
--
-- Desc{name, inflator, subDescs, [infObj, lockObj, parentDesc, scope]}
-- name: the function name to generte.
-- lockObj: lock obj
-- infObj:  inflate object
-- inflator: the inflator function callback. often  '(desc, ...)'
-- subDescs: sub desc
function untis_configBuilder(g_desc)
	--gen lua code for config
	registerFunc(g_desc);

	if(g_desc.subDescs) then
		for i = 1, #(g_desc.subDescs) do
			untis_configBuilder(g_desc.subDescs[i]);
		end
	end
end


local desc = newApiDesc()
desc.name = "configuration"
desc.inflator = function(desc, t)
	print("- inflator desc args: ", t)
end

local desc1 = newApiDesc()
desc1.name = "options"
desc1.inflator = function(desc, t)
	print("- inflator desc1 args: ", t)
end

local desc2 = newApiDesc()
desc2.name = "includes"
desc2.inflator = function(desc, t)
	print("- inflator desc2 args: ", t)
end

desc.addChild(desc1)
desc1.addChild(desc2)


untis_configBuilder(desc)

configuration "config123"
options{
	"1", "2", "3"
}
includes("--include")

