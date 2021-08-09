--[[
    Buff 视图

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class BufferView
local BufferView = Class("BufferView")

---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
---@param posx number
---@param posy number
---@param cursorx number
---@param cursory number
function BufferView:init(buffer, width, height, posx, posy, cursorx, cursory)
    cursorx = cursorx or 0
    cursory = cursory or 0

    self.buffer = buffer
    self.width = width
    self.height = height
    self.posx = posx
    self.posy = posy
    self.cursorx = 0
    self.cursory = 0
    self.offsetx = 0
    self.offsety = 0
end

---@param window Window
function BufferView:render(window)
end

---@param key number
function BufferView:processKey(key)
end

return BufferView
