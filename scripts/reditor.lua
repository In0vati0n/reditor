--[[
    REditor C 暴露给 Lua API 辅助文件

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class _c_reditor
reditor = {}

--------------------------------------------
---REditor
--------------------------------------------

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

---终止程序
---@param msg string
function reditor.die(msg)
end

---终止程序
---@param code number
function reditor.exit(code)
end

--------------------------------------------
---argv
--------------------------------------------

---程序启动参数
---@type string[]
reditor.argv = {}

--------------------------------------------
---REditor render
--------------------------------------------

---@type _c_t_render
reditor.t_render = {}

---@class _c_t_render
t_render = {}
---@param s string
function t_render.draw(s)
end

---@return number, number
function t_render.getWindowSize()
end

--------------------------------------------
---REditor input
--------------------------------------------

---@type _c_input
reditor.input = {}

---@class _c_input
input = {}

---@type number
input.BACKSPACE = 0
---@type number
input.ARROW_LEFT = 0
---@type number
input.ARROW_RIGHT = 0
---@type number
input.ARROW_UP = 0
---@type number
input.ARROW_DOWN = 0
---@type number
input.DEL_KEY = 0
---@type number
input.HOME_KEY = 0
---@type number
input.END_KEY = 0
---@type number
input.PAGE_UP = 0
---@type number
input.PAGE_DOWN = 0

--------------------------------------------
---REditor t input
--------------------------------------------

---@type _c_t_input
reditor.t_input = {}

---@class _c_t_input
t_input = {}

---@param s number|string
---@return number
function t_input.ctrlKey(s)
end

---@return number
function t_input.readKey()
end

--------------------------------------------
---REditor base type
--------------------------------------------
---@class Rect
---@field x number
---@field y number
---@field width number
---@field height number
local rect = {}