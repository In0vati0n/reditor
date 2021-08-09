--[[
    Lua 程序启动入口

    @Author In0vati0n
    @Date 2021/08/08
--]]
--region 初始化

--------------------------------------------
---初始化全局变量
--------------------------------------------

---@type string
_G.WORK_DIR = reditor.getWorkDir()

---配置脚本查找路径
package.path = package.path .. WORK_DIR .. [[/?.lua;]]
package.path = package.path .. WORK_DIR .. [[/scripts/?.lua;]]

---@class Class
_G.Class = require("libs.class.30log")
---@class Array
_G.Array = require("libs.array.array")

require("utils.math_utils")

function logError(...)
    logErrorF("%s", table.concat({...}, " "))
end

function logErrorF(f, ...)
    print("Lua Error: " .. string.format(f, ...) .. "\n" .. debug.traceback())
end

--------------------------------------------
---初始化全局变量结束
--------------------------------------------

--endregion

local status, _ = xpcall(require, logError, "main")
if not status then
    reditor.die("require main error")
end

local status, ret = xpcall(require, logError, "config")
if not status then
    reditor.die("require config error")
end

---@type GlobalConfig
_G.GlobalConfig = ret

return function()
    local func

    local earlyInit = function()
        local ret = xpcall(onLoad, logError, reditor.argv)
        if not ret then
            return 1
        end

        func = onUpdate
    end

    func = earlyInit

    while func do
        local _, retval = xpcall(func, logError)
        if retval then
            return retval
        end
        coroutine.yield()
    end

    return 1
end
