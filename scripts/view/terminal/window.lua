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

local tr = reditor.t_render
local ti = reditor.t_input
local input = reditor.input

---@param editor Editor
function TerminalWindow:init(editor)
    Window.init(self, editor)

    self.width, self.height = tr.getWindowSize()

    self.views = Array()

    ---@private
    self._sb = StringBuilder()

    if editor.currentActiveBuffer then
        ---@type TerminalBufferView
        self.curtActiveBuffer = TerminalBufferView(editor.currentActiveBuffer, self.width, self.height, 0, 0)
        self.views:add(self.curtActiveBuffer)
    end
end

function TerminalWindow:onUpdate()
    ---渲染窗体
    self:_render()

    ---处理输入事件
    self:_processEvent()
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
function TerminalWindow:_render()
    ---清空屏幕
    self:_clear()

    ---渲染每一个显示的buffer
    ---@param view TerminalBufferView
    for i, view in self.views:iparis() do
        view:render(self)
    end

    ---根据当前激活的 buffer 渲染光标位置
    if self.curtActiveBuffer then
        self:setCursorPos(self.curtActiveBuffer.cursory, self.curtActiveBuffer.cursorx)
    end

    ---将内容输出到终端
    tr.draw(self._sb:tostring(true))
end

---@private
function TerminalWindow:_processEvent()
    local key = ti.readKey()

    if key == ti.ctrlKey("q") then
        reditor.exit(0)
        return
    end

    if self.curtActiveBuffer then
        self.curtActiveBuffer:processKey(key)
    end
end

---@private
function TerminalWindow:_clear()
    self._sb:append("\x1b[2J")
    self._sb:append("\x1b[H")
end

return TerminalWindow
