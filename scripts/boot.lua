--[[
    Lua 程序启动入口

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@type string
WORK_DIR = reditor.getWorkDir()

---配置脚本查找路径
package.path = package.path .. WORK_DIR .. [[/?.lua;]]
package.path = package.path .. WORK_DIR .. [[/scripts/?.lua;]]

function logError(...)
    logErrorF("%s", table.concat({...}, " "))
end

function logErrorF(f, ...)
    print("Error: " .. string.format(f, ...))
end

require("main")

return function()
    local function errorHandle(...)
        logError(...)
    end

    while update do
        local _, retval = xpcall(update, errorHandle)
        if retval then
            return retval
        end
        coroutine.yield()
    end

    return 1
end
