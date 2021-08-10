--[[
    Buff 视图

    @Author In0vati0n
    @Date 2021/08/08
--]]
local BufferView = require("view.base.buffer_view")

---@class TerminalBufferView : BufferView
local TerminalBufferView = BufferView:extend("TerminalBufferView")

local input = reditor.input

---@param window Window
---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
---@param posx number
---@param posy number
---@param mode Mode
---@param cursorx number|nil
---@param cursory number|nil
function TerminalBufferView:init(window, buffer, width, height, posx, posy, mode, cursorx, cursory)
    BufferView.init(self, window, buffer, width, height, posx, posy, mode, cursorx, cursory)

    self.msg = "Ctrl - Q QUIT"
end

function TerminalBufferView:render()
    ---渲染内容
    ---留出两行，一行给状态栏，一行给信息栏
    local contentHeight = self.height - 2
    self.mode:render()

    ---渲染状态栏
    self:_drawStatusBar(self.posx, self.posy + contentHeight + 1)

    ---渲染信息栏
    self:_drawMessageBar(self.posx, self.posy + contentHeight + 2)
end

---@param key number
function TerminalBufferView:processKey(key)
    BufferView.processKey(self, key)
end

---@private
---@param window Window
function TerminalBufferView:_drawStatusBar(posx, posy)
    local leftHint = string.format("- [%s]", self.mode:getName())
    local rightHint = ""

    local whiteCnt = self.width - #leftHint - #rightHint

    self.window:setCursorPos(posx, posy)
    self.window:draw("\x1b[7m")
    self.window:draw(leftHint)
    while whiteCnt > 0 do
        self.window:draw(" ")
        whiteCnt = whiteCnt - 1
    end
    self.window:draw(rightHint)
    self.window:draw("\x1b[m")
end

---@private
---@param window Window
function TerminalBufferView:_drawMessageBar(posx, posy)
    self.window:setCursorPos(posx, posy)
    self.window:draw("  ")
    self.window:draw(self.msg)
end

return TerminalBufferView
