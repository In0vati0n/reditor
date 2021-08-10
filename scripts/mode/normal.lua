--[[
    普通编辑模式

    @Author In0vati0n
    @Date 2021/08/09
--]]
local Mode = require("mode.mode")

local input = reditor.input
local tinput = reditor.t_input

---@class NormalMode : Mode
local NormalMode = Mode:extend("NormalMode")

---@param bufferView BufferView
function NormalMode:init(bufferView)
    Mode.init(self, bufferView)

    self.offsetx = 0
    self.offsety = 0
end

---@param x number
---@param y number
---@param width number
---@param height number
---@param window Window
function NormalMode:render(x, y, width, height, window)
    local rows = self.bufferView.buffer.file.rows
    local filerow = 0
    local maxwidth = rows:size() == 0 and math.numberOfDigits(window.height) or math.numberOfDigits(rows:size())

    for i = 1, height do
        filerow = i + self.offsety

        if filerow > rows:size() then
            window:draw(string.format("%" .. maxwidth .. "d", filerow))

            if rows:size() == 0 and filerow == math.floor(height / 3) then
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
            local finalLen = len > width - maxwidth - 2 and width - maxwidth - 2 or len

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
function NormalMode:processKey(key)
    if key == tinput.ctrlKey("q") then
        reditor.exit(0)
        return
    end

    if key == input.ARROW_DOWN then
        self.bufferView:moveCursor(0, 1)
    elseif key == input.ARROW_UP then
        self.bufferView:moveCursor(0, -1)
    elseif key == input.ARROW_RIGHT then
        self.bufferView:moveCursor(1, 0)
    elseif key == input.ARROW_LEFT then
        self.bufferView:moveCursor(-1, 0)
    end
end

---@return string
function NormalMode:getName()
    return "Normal"
end

return NormalMode
