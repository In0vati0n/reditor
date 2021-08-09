--[[
    数学辅助方法

    @Author In0vati0n
    @Date 2021/08/09
--]]
---@param n number
---@return number
function math.numberOfDigits(n)
    if n > 0 then
        return #tostring(n)
    else
        return #tostring(n) - 1
    end
end
