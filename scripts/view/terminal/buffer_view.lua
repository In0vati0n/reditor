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
end

---@param window TerminalWindow
function TerminalBufferView:render(window)
    window:setCursorPos(self.posx, self.posy)

    ---渲染内容
    ---留出两行，一行给状态栏，一行给信息栏
    local contentHeight = window.height - 2
    self.mode:render(self.posx, self.posy, self.width, contentHeight, window)
end

---@param key number
function TerminalBufferView:processKey(key)
    BufferView.processKey(self, key)
end

return TerminalBufferView
