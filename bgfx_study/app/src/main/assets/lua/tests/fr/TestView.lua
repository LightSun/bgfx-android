---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/11/7 0007 下午 6:28
---
local as = require("fr.AppStarter")
local view = require("fr.View")
local uiCore = require("fr.UiCore")

local function newTestView()
    local self = view.new("TestView")
    local textStyle;
    local color1;

    function self.onInitialize(ctx)
        color1 = uiCore.newColor(0xff000000); --rgba
        textStyle = uiCore.newTextStyle()
        textStyle.size(36)
                .color(uiCore.newColor(0xFFFFFFFF))
                .font(uiCore.newFont(ctx, "droidsans", "runtime/font/droidsans.ttf"))
    end

    function self.onDestroy()
        color1 = nil;
        textStyle = nil;
    end

    function self.onDraw(canvas)
        --canvas.save()
                --.translate(100, 100);
        canvas.beginPath()
            .roundedRect(50,50,100,100,10)
            .fillColor(color1)
            .fill();

        -- Draw styled text
--[[        canvas.textStyle(textStyle)
            .beginPath()
            .fillText("Hello Canvas",30,190);]]
        --canvas.restore();
    end

    return self;
end
as.startViews(nil, newTestView())