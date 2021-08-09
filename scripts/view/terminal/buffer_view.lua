--[[
    Buff 视图

    @Author In0vati0n
    @Date 2021/08/08
--]]
local BufferView = require("view.base.buffer_view")

---@class TerminalBufferView : BufferView
local TerminalBufferView = BufferView:extend("TerminalBufferView")

local input = reditor.input

---@param buffer Buffer
---@param window Window
---@param width number
---@param height number
function TerminalBufferView:init(buffer, width, height, posx, posy)
    BufferView.init(self, buffer, width, height, posx, posy)
end

---@param window TerminalWindow
function TerminalBufferView:render(window)
    window:setCursorPos(self.posx, self.posy)

    local rows = self.buffer.file.rows
    local filerow = 0
    local maxwidth = math.numberOfDigits(rows:size())

    for i = 1, window.height do
        filerow = i + self.offsety

        if filerow > rows:size() then
            window:draw(string.format("%" .. maxwidth .. "d", filerow))

            if rows:size() == 0 and filerow == math.floor(window.height / 3) then
                local welcome = string.format("~ %s %s ~", _G.GlobalConfig.welcomeLabel, _G.GlobalConfig.version)
                local padding = (window.width - #welcome) / 2
                padding = padding - #string.format("%" .. maxwidth .. "d", filerow)
                while padding > 0 do
                    window:draw(" ")
                    padding = padding - 1
                end
                window:draw(welcome)
            end
        else
            local row = rows:get(filerow)
            local chars = row.chars
            local len = #chars
            local finalLen = len > self.width - maxwidth - 2 and self.width - maxwidth - 2 or len

            window:draw(string.format("%" .. maxwidth .. "d  ", filerow))

            if finalLen ~= len then
                window:draw(chars:sub(0, finalLen))
            else
                window:draw(chars)
            end
        end

        if i < window.height then
            window:draw("\r\n")
        end
    end
end

---@param key number
function TerminalBufferView:processKey(key)
    if key == input.ARROW_DOWN then
        self.cursory = self.cursory + 1
    end
end

return TerminalBufferView
