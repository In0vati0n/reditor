--[[
    普通编辑模式

    @Author In0vati0n
    @Date 2021/08/09
--]]
local Mode = require("mode.mode")
local StringBuilder = require("libs.stringbuilder.stringbuilder")

local input = reditor.input
local tinput = reditor.t_input

---@class NormalMode : Mode
local NormalMode = Mode:extend("NormalMode")

local maxwidth = 0
local sb = StringBuilder()

---@param bufferView BufferView
function NormalMode:init(bufferView)
    Mode.init(self, bufferView)

    local rows = self.bufferView.buffer.file.rows
    maxwidth = rows:size() == 0 and math.numberOfDigits(bufferView.height) or math.numberOfDigits(rows:size())
    bufferView:setEditRect(maxwidth + 1, 0, bufferView.width - maxwidth - 1, bufferView.height)
end

function NormalMode:render()
    local view = self.bufferView
    local rows = self.bufferView.buffer.file.rows
    local filerow = 0

    for i = 1, view.editRect.height do
        filerow = i + view.offsety

        self.bufferView:drawNoneditableLine(i, string.format("%" .. maxwidth .. "d", filerow))

        if filerow > rows:size() then
            if rows:size() == 0 and filerow == math.floor(view.editRect.height / 3) then
                local welcome = string.format("~ %s %s ~", _G.GlobalConfig.welcomeLabel, _G.GlobalConfig.version)
                local padding = (view.editRect.width - #welcome) / 2
                padding = padding - #string.format("%" .. maxwidth .. "d", filerow)
                while padding > 0 do
                    sb:append(" ")
                    padding = padding - 1
                end
                sb:append(welcome)
                view:drawLine(i, sb:tostring(true))
            end
        else
            ---@type Row
            local row = rows:get(filerow)
            view:drawLine(i, row.chars)
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
