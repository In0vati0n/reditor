--[[
    编辑模式

    @Author In0vati0n
    @Date 2021/08/09
--]]
---@class Mode
local Mode = Class("Mode")

---@param bufferView BufferView
function Mode:init(bufferView)
    self.bufferView = bufferView
end

---@param x number
---@param y number
---@param width number
---@param height number
---@param window Window
function Mode:render(x, y, width, height, window)
end

---@param key number
function Mode:processKey(key)
end

return Mode
