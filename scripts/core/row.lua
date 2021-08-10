--[[
    Row

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class Row
local Row = Class("Row")

---@param chars string
function Row:init(chars)
    if chars and chars:match("(.*)\r\n$") then
        self.chars = chars:sub(0, -3)
    else
        self.chars = chars
    end
    self.render = ""
end

---@return number
function Row:length()
    return self.chars and #self.chars or 0
end

return Row
