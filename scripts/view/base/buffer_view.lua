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
    cursorx = cursorx or 1
    cursory = cursory or 1

    self.buffer = buffer

    self.width = width
    self.height = height
    self.posx = posx
    self.posy = posy

    self.cursorx = cursorx
    self.cursory = cursory

    self.editRect = {0, 0, 0, 0}

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
    self.editRect = {x, y, width, height}
end

--endregion

---@param x number
---@param y number
function BufferView:moveCursor(x, y)
    if self:getRowCnt() == 0 then
        return
    end

    local sign = math.sign(x)
    while x ~= 0 do
        self:_moveCursor(sign, 0)
        x = x - sign
    end

    sign = math.sign(y)
    while y ~= 0 do
        self:_moveCursor(0, sign)
        y = y - sign
    end
end

---移动光标
---x y 必须为 1 或 -1
---@private
---@param x number
---@param y number
function BufferView:_moveCursor(x, y)
    if x ~= 0 then
        if x < 0 then
            if self.cursorx ~= 0 then
                self.cursorx = self.cursorx - 1
            elseif self.cursory > 0 then
                self.cursory = self.cursory - 1
                self.cursorx = self:getRow(self.cursory):length()
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
        if y < 0 then
            if self.cursory ~= 0 then
                self.cursory = self.cursory - 1
            end
        else
            if self.cursory < self:getRowCnt() then
                self.cursory = self.cursory + 1
            end
        end
    end
end

return BufferView
