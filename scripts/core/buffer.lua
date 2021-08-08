--[[
    Buffer

    @Author In0vati0n
    @Date 2021/08/08
--]]
---@class Buffer
local Buffer = Class("Buffer")

---@param id number
---@param file File
function Buffer:init(id, file)
    self.id = id

    self.file = file

    self.isActive = false
    self.isShow = false
end

function Buffer:onCreate()
end

function Buffer:onClose()
end

function Buffer:onShow()
    self.isShow = true
end

function Buffer:onHide()
    self.isShow = false
end

function Buffer:onActive()
    self.isActive = true
end

function Buffer:onDeactive()
    self.isActive = false
end

return Buffer
