local bgfx = require("bgfx");
--todo pre init
_G.app_init = function ()
   bgfx.getInit(nil).resution.reset = 0x00000080; --vsync
   bgfx.setDebug(0x00000008);
end
_G.app_draw = function ()
   return 0;
end
_G.app_destroy = function ()

end
local app = bgfx.newApp("app_init", "app_draw", "app_destroy");
app:startLoop();