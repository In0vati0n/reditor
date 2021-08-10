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

function Mode:render()
end

---@param key number
function Mode:processKey(key)
end

---@return string
function Mode:getName()
end

return Mode
