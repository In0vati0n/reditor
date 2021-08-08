--[[
    Buff 视图

    @Author In0vati0n
    @Date 2021/08/08
--]]
local BufferView = require("view.base.buffer_view")

---@class TerminalBufferView : BufferView
local TerminalBufferView = BufferView:extend("TerminalBufferView")

---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
function TerminalBufferView:init(buffer, width, height, posx, posy)
    BufferView.init(self, buffer, width, height, posx, posy)
end

---@param window TerminalWindow
function TerminalBufferView:render(window)
    window:setCursorPos(self.posx, self.posy)

    ---@param row Row
    for i, row in self.buffer.file.rows:iparis() do
        print(i, row.chars)
        local chars = row.chars
        local len = #chars
        local finalLen = len > self.width and self.width or len
        if finalLen ~= len then
            window:draw(chars:sub(0, finalLen))
        else
            window:draw(chars)
        end

        window:draw("\r\n")
    end
end

return TerminalBufferView
