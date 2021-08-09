--[[
    Buff 视图

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class BufferView
local BufferView = Class("BufferView")

---工厂方法
---创建一个 BufferView
---@param type Class
---@param modeType Class
---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
---@param posx number
---@param posy number
---@param mode Mode
---@param cursorx number|nil
---@param cursory number|nil
function BufferView.create(type, modeType, buffer, width, height, posx, posy, cursorx, cursory)
    local bufferView = type(buffer, width, height, posx, posy, cursorx, cursory)
    local mode = modeType(bufferView)
    bufferView.mode = mode
    return bufferView
end

---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
---@param posx number
---@param posy number
---@param mode Mode
---@param cursorx number|nil
---@param cursory number|nil
function BufferView:init(buffer, width, height, posx, posy, cursorx, cursory)
    cursorx = cursorx or 0
    cursory = cursory or 0

    self.buffer = buffer
    self.width = width
    self.height = height
    self.posx = posx
    self.posy = posy
    self.cursorx = 0
    self.cursory = 0
    self.offsetx = 0
    self.offsety = 0

    ---@type Mode
    self.mode = nil
end

---@param window Window
function BufferView:render(window)
end

---@param key number
function BufferView:processKey(key)
    self.mode:processKey(key)
end

return BufferView
