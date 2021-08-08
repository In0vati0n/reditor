--[[
    终端窗体

    @Author In0vati0n
    @Date 2021/08/08
--]]

local Window = require('view.window')

---@class TerminalWindow
local TerminalWindow = Window:extend('TerminalWindow')

---@param editor Editor
function TerminalWindow:init(editor)
    Window.init(self, editor)
end

function TerminalWindow:update()
    -- reditor.refreshScreen()

    local s = "\x1b[2J\x1b[HHello Lua"
    reditor.terminal_render.draw(s)

    reditor.processKeypress()
end

return TerminalWindow