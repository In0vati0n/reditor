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
---@param window Window
---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
---@param posx number
---@param posy number
---@param mode Mode
---@param cursorx number|nil
---@param cursory number|nil
function BufferView.create(type, modeType, window, buffer, width, height, posx, posy, cursorx, cursory)
    local bufferView = type(window, buffer, width, height, posx, posy, cursorx, cursory)
    local mode = modeType(bufferView)
    bufferView.mode = mode
    return bufferView
end

---@param window Window
---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
---@param posx number
---@param posy number
---@param mode Mode
---@param cursorx number|nil
---@param cursory number|nil
function BufferView:init(window, buffer, width, height, posx, posy, cursorx, cursory)
    cursorx = cursorx or 1
    cursory = cursory or 1

    self.window = window
    self.buffer = buffer

    self.width = width
    self.height = height
    self.posx = posx
    self.posy = posy

    self.cursorx = cursorx
    self.cursory = cursory

    self.offsetx = 0
    self.offsety = 0

    ---@type Rect
    self.editRect = {
        x = 0,
        y = 0,
        width = 0,
        height = 0
    }

    ---@type Mode
    self.mode = nil
end

function BufferView:render()
end

---@param key number
function BufferView:processKey(key)
    self.mode:processKey(key)
end

--region Get

---@param i number
---@return Row
function BufferView:getRow(i)
    return self.buffer.file.rows:get(i)
end

---@return number
function BufferView:getRowCnt()
    return self.buffer.file.rows:size()
end

--endregion

--region Set

---设置可编辑区域
---@param x number
---@param y number
---@param width number
---@param height number
function BufferView:setEditRect(x, y, width, height)
    x = x > self.width and self.width or x
    y = y > self.height and self.height or y
    height = height >= self.height - 2 - y and self.height - 2 - y or height
    width = width > self.width - x and self.width - x or width
    self.editRect = {
        x = x,
        y = y,
        width = width,
        height = height
    }
end

--endregion

---@param offsetx number
---@param offsety number
function BufferView:moveCursor(offsetx, offsety)
    if self:getRowCnt() == 0 then
        return
    end

    local sign = math.sign(offsetx)
    while offsetx ~= 0 do
        self:_moveCursor(sign, 0)
        offsetx = offsetx - sign
    end

    sign = math.sign(offsety)
    while offsety ~= 0 do
        self:_moveCursor(0, sign)
        offsety = offsety - sign
    end
end

---@param y number
---@param chars string
function BufferView:drawNoneditableLine(y, chars)
    self.window:setCursorPos(self.posx, self.posy + y)
    if #chars > self.editRect.x then
        self.window:draw(chars:sub(0, self.editRect.x))
    else
        self.window:draw(chars)
    end
end

---@param y number
---@param chars string
function BufferView:drawLine(y, chars)
    self.window:setCursorPos(self.posx + self.editRect.x + 1, self.posy + self.editRect.y + y)
    if #chars > self.editRect.width then
        self.window:draw(chars:sub(0, self.editRect.width))
    else
        self.window:draw(chars)
    end
end

---移动光标
---x y 必须为 1 或 -1
---@private
---@param offsetx number
---@param offsety number
function BufferView:_moveCursor(offsetx, offsety)
    if offsetx ~= 0 then
        if offsetx < 0 then
            if self.cursorx ~= 1 then
                self.cursorx = self.cursorx - 1
            elseif self.cursory > 1 then
                self.cursory = self.cursory - 1
                self.cursorx = math.max(1, self:getRow(self.cursory):length())
            end
        else
            local row = self:getRow(self.cursory)
            if self.cursorx < row:length() then
                self.cursorx = self.cursorx + 1
            elseif self.cursory + 1 <= self:getRowCnt() then
                self.cursory = self.cursory + 1
                self.cursorx = 1
            end
        end
    else
        if offsety < 0 then
            if self.cursory ~= 1 then
                self.cursory = self.cursory - 1
            end
        else
            if self.cursory < self:getRowCnt() then
                self.cursory = self.cursory + 1
            end
        end

        local row = self:getRow(self.cursory)
        if self.cursorx > row:length() then
            self.cursorx = math.max(1, row:length())
        end
    end

    self:_viewScroll()
end

---@private
function BufferView:_viewScroll()
    if self.cursory < self.offsety then
        self.offsety = self.cursory
    end

    if self.cursory >= self.offsety + self.editRect.height then
        self.offsety = self.cursory - self.editRect.height + 1
    end
end

return BufferView
