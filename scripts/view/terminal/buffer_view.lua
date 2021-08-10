--[[
    Buff 视图

    @Author In0vati0n
    @Date 2021/08/08
--]]
local BufferView = require("view.base.buffer_view")

---@class TerminalBufferView : BufferView
local TerminalBufferView = BufferView:extend("TerminalBufferView")

local input = reditor.input

---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
function TerminalBufferView:init(buffer, width, height, posx, posy, mode, cursorx, cursory)
    BufferView.init(self, buffer, width, height, posx, posy, mode, cursorx, cursory)

    self.msg = "Ctrl - Q QUIT"
end

---@param window TerminalWindow
function TerminalBufferView:render(window)
    window:setCursorPos(self.posx, self.posy)

    ---渲染内容
    ---留出两行，一行给状态栏，一行给信息栏
    local contentHeight = window.height - 2
    self.mode:render(self.posx, self.posy, self.width, contentHeight, window)

    ---渲染状态栏
    self:_drawStatusBar(window, self.posx, self.posy + contentHeight + 1)

    ---渲染信息栏
    self:_drawMessageBar(window, self.posx, self.posy + contentHeight + 2)
end

---@param key number
function TerminalBufferView:processKey(key)
    BufferView.processKey(self, key)
end

---@private
---@param window Window
function TerminalBufferView:_drawStatusBar(window, posx, posy)
    local leftHint = string.format("- [%s]", self.mode:getName())
    local rightHint = ""

    local whiteCnt = self.width - #leftHint - #rightHint

    window:setCursorPos(posx, posy)
    window:draw("\x1b[7m")
    window:draw(leftHint)
    while whiteCnt > 0 do
        window:draw(" ")
        whiteCnt = whiteCnt - 1
    end
    window:draw(rightHint)
    window:draw("\x1b[m")
end

---@private
---@param window Window
function TerminalBufferView:_drawMessageBar(window, posx, posy)
    window:setCursorPos(posx, posy)
    window:draw("  ")
    window:draw(self.msg)
end

return TerminalBufferView
