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
end

---@param key number
function NormalMode:processKey(key)
    if key == tinput.ctrlKey("q") then
        reditor.exit(0)
        return
    end

    if key == input.ARROW_DOWN then
        self.bufferView.cursory = self.bufferView.cursory + 1
    elseif key == input.ARROW_UP then
        self.bufferView.cursory = self.bufferView.cursory - 1
    end
end

return NormalMode
