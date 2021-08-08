--[[
    Lua 程序主入口

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class ArgParser
local ArgParser = require("libs.argparse.src.argparse")
local PathUtils = require("utils.path_utils")
local Editor = require("core.editor")
local TerminalWindow = require("view.terminal.window")

---@type Editor
local editor = Editor()

---@type Array
local windows = Array()

---渲染线程
---@type Array
local renderThreads = Array()

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
    local file = editor:openFile(path)
    local buffer = editor:createBuffer(file)
    editor:showBuffer(buffer)
    editor:activeBuffer(buffer)

    local tw = TerminalWindow(editor)
    windows:add(tw)

    renderThreads:add(
        coroutine.create(
            function()
                while true do
                    tw:onUpdate()
                    coroutine.yield()
                end
            end
        )
    )

    return status
end

function onUpdate()
    for i, thread in renderThreads:iparis() do
        coroutine.resume(thread)
    end
end
