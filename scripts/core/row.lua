--[[
    Row

    @Author In0vati0n
    @Date 2021/08/08
--]]

---@class Row
local Row = Class('Row')

---@param chars string
function Row:init(chars)
    self.chars = chars
    self.render = ''
end

return Row