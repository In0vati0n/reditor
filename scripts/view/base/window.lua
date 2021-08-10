--[[
    窗体基类

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class Window
local Window = Class("Window")

---@param editor Editor
function Window:init(editor)
    self.editor = editor
end

function Window:onUpdate()
end

---@param chars string
function Window:draw(chars)
end

---@param x number
---@param y number
function Window:setCursorPos(x, y)
end

return Window
