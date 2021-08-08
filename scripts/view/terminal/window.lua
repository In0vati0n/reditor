--[[
    终端窗体

    @Author In0vati0n
    @Date 2021/08/08
--]]
local Window = require("view.base.window")
local TerminalBufferView = require("view.terminal.buffer_view")
local StringBuilder = require("libs.stringbuilder.stringbuilder")

---@class TerminalWindow
local TerminalWindow = Window:extend("TerminalWindow")

local tr = reditor.terminal_render

---@param editor Editor
function TerminalWindow:init(editor)
    Window.init(self, editor)

    self.width, self.height = tr.getWindowSize()

    self.views = Array()

    ---@private
    self._sb = StringBuilder()

    if editor.currentActiveBuffer then
        self.views:add(TerminalBufferView(editor.currentActiveBuffer, self.width, self.height, 0, 0))
    end
end

function TerminalWindow:onUpdate()
    self:_clear()

    ---@param view TerminalBufferView
    for i, view in self.views:iparis() do
        view:render(self)
    end

    tr.draw(self._sb:tostring(true))

    reditor.processKeypress()
end

---@param chars string
function TerminalWindow:draw(chars)
    self._sb:append(chars)
end

---@param x number
---@param y number
function TerminalWindow:setCursorPos(x, y)
    self._sb:append_format("\x1b[%d;%dH", x, y)
end

---@private
function TerminalWindow:_clear()
    self._sb:append("\x1b[2J")
    self._sb:append("\x1b[H")
end

return TerminalWindow
