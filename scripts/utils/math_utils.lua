--[[
    数学辅助方法

    @Author In0vati0n
    @Date 2021/08/09
--]]
---获取整数位数
---@param n number
---@return number
function math.numberOfDigits(n)
    n = math.floor(n)
    if n > 0 then
        return #tostring(n)
    else
        return #tostring(n) - 1
    end
end

function math.sign(x)
    return x > 0 and 1 or (x == 0 and 0 or -1)
end
