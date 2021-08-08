--[[
    Lua 程序主入口

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class ArgParser
local ArgParser = require("libs.argparse.src.argparse")
local PathUtils = require("utils.path_utils")

---@type Editor
_G.Editor = require("core.editor"):new()

function onLoad(argv)
    local parser = ArgParser("reditor")
    parser:argument("file", "Input file."):args("?")

    local parse = function()
        return parser:parse(argv)
    end

    local status, args = xpcall(parse, logError)
    if not status then
        return status
    end

    local path = args["file"] and args["file"] or ""
    local file = _G.Editor:openFile(path)
    local buffer = _G.Editor:createBuffer(file)
    _G.Editor:showBuffer(buffer)
    _G.Editor:activeBuffer(buffer)

    return status
end

function onUpdate()
    reditor.refreshScreen()
    reditor.processKeypress()
end
