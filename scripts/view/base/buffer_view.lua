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
function BufferView:init(buffer, width, height, posx, posy)
    self.buffer = buffer
    self.width = width
    self.height = height
    self.posx = posx
    self.posy = posy
end

---@param window Window
function BufferView:render(window)
end

return BufferView
