--
-- log.lua
--
-- Copyright (c) 2016 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

---@alias LogFunc fun(...):void
---@alias LogFormatFunc fun(format: string, ...):void

---@class Log
---@field public trace LogFunc
---@field public debug LogFunc
---@field public info LogFunc
---@field public warn LogFunc
---@field public error LogFunc
---@field public fatal LogFunc
---@field public traceFormat LogFormatFunc
---@field public debugFormat LogFormatFunc
---@field public infoFormat LogFormatFunc
---@field public warnFormat LogFormatFunc
---@field public errorFormat LogFormatFunc
---@field public fatalFormat LogFormatFunc
local log = { _version = "0.1.1" }

log.usecolor = true
log.outfile = nil
log.modes = {
    TRACE = "trace",
    DEBUG = "debug",
    INFO = "info",
    WARN = "warn",
    ERROR = "error",
    FATAL = "fatal",
}

log.level = log.modes.TRACE

log._handlers = {}

---@param level string log level
---@param handler fun(msg: string) handler
function log.setLogHandler(level, handler)
    log._handlers[level] = handler
end

local modes = {
    { name = log.modes.TRACE, color = "\27[34m" },
    { name = log.modes.DEBUG, color = "\27[36m" },
    { name = log.modes.INFO, color = "\27[32m" },
    { name = log.modes.WARN, color = "\27[33m" },
    { name = log.modes.ERROR, color = "\27[31m" },
    { name = log.modes.FATAL, color = "\27[35m" }
}

local levels = {}
for i, v in ipairs(modes) do
    levels[v.name] = i
end

local round = function(x, increment)
    increment = increment or 1
    x = x / increment
    return (x > 0 and math.floor(x + .5) or math.ceil(x - .5)) * increment
end

local _tostring = tostring

local tostring = function(...)
    local t = {}
    for i = 1, select("#", ...) do
        local x = select(i, ...)
        if type(x) == "number" then
            x = round(x, .01)
        end
        t[#t + 1] = _tostring(x)
    end
    return table.concat(t, " ")
end

for i, x in ipairs(modes) do
    local nameupper = x.name:upper()
    log[x.name] = function(...)
        -- Return early if we're below the log level
        if i < levels[log.level] then
            return
        end

        local msg = tostring(...)
        local info = debug.getinfo(2, "Sl")
        local lineinfo = info.short_src .. ":" .. info.currentline

        local outputContent = string.format(
                "%s[%-6s%s]%s %s: %s",
                log.usecolor and x.color or "",
                nameupper,
                os.date("%H:%M:%S"),
                log.usecolor and "\27[0m" or "",
                lineinfo,
                msg
        )

        -- Output to console
        if log._handlers[x.name] then
            log._handlers[x.name](outputContent)
        else
            print(outputContent)
        end

        -- Output to log file
        if log.outfile then
            local fp = io.open(log.outfile, "a")
            local str = string.format("[%-6s%s] %s: %s\n", nameupper, os.date(), lineinfo, msg)
            fp:write(str)
            fp:close()
        end
    end

    log[x.name .. "Format"] = function(format, ...)
        log[x.name](string.format(format, ...))
    end
end

return log
