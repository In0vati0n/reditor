--[[
    Lua 程序主入口

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class ArgParser
local ArgParser = require("libs.argparse.src.argparse")
local PathUtils = require("utils.path_utils")
local Editor = require("core.editor")
local TerminalWindow = require("view.terminal_window")

---@type Editor
local editor = Editor()

---@type Array
local windows = Array()

---渲染线程
---@type thread
local renderThread =
    coroutine.create(
    function()
        while true do
            ---@param win Window
            for i, win in windows:iparis() do
                win:update()
            end

            coroutine.yield()
        end
    end
)

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

    windows:add(TerminalWindow(editor))

    return status
end

function onUpdate()
    coroutine.resume(renderThread)
end
