--- @type string
WORK_DIR = reditor.getWorkDir()
package.path = package.path .. WORK_DIR .. [[/?.lua;]]
package.path = package.path .. WORK_DIR .. [[/scripts/?.lua;]]

function log_error(...)
    logf_error("%s", table.concat({...}, " "))
end

function logf_error(f, ...)
    print("Error: " .. string.format(f, ...))
end

require("main")

return function()
    local function errorHandle(...)
        log_error(...)
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
