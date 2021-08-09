--[[
    REditor C 暴露给 Lua API 辅助文件

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class _c_reditor
reditor = {}

---得到当前工作目录
---@return string
function reditor.getWorkDir()
end

---刷新屏幕
function reditor.refreshScreen()
end

---处理按键
function reditor.processKeypress()
end

---程序启动参数
---@type string[]
reditor.argv = {}

---@type _c_terminal_render
reditor.terminal_render = {}

---@class _c_terminal_render
tr = {}
---@param s string
function tr.draw(s)
end

---@return number, number
function tr.getWindowSize()
end

---@type _c_input
reditor.input = {}

---@class _c_input
input = {}

---@param s number|string
---@return number
function input.ctrlKey(s)
end
