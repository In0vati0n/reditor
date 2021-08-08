--[[
    Editor

    @Author In0vati0n
    @Date 2021/08/08
--]]
local File = require("core.file")
local Buffer = require("core.buffer")

---@class Editor
local Editor = Class("Editor")

function Editor:init()
    ---@type table<number, File>
    self.files = {}

    self.fileId = 0
    self.bufferId = 0

    ---@type Array
    self.bufferArr = Array()
    ---@type Array
    self.showBufferArr = Array()
    ---@type Buffer
    self.currentActiveBuffer = nil
end

---打开一个文件
---@param path string
---@return File
function Editor:openFile(path)
    local file = File:new(self:_fileId(), path)
    return file
end

---创建一个Buffer
---@param file File
---@return Buffer
function Editor:createBuffer(file)
    local buffer = Buffer:new(self:_bufferId(), file)
    self.bufferArr:add(buffer)
    buffer:onCreate()
    return buffer
end

---关闭一个Buffer
---@param buffer Buffer
function Editor:closeBuffer(buffer)
    buffer:onClose()
    self.bufferArr:remove(buffer)

    ---TODO: file 使用引用计数，当没有 buffer 引用它需要将其释放
end

---将 Buffer 显示
---@param buffer Buffer
---@return boolean
function Editor:showBuffer(buffer)
    if buffer.isShow then
        return false
    end

    self.showBufferArr:add(buffer)
    buffer:onShow()
    return true
end

---将 Buffer 隐藏
---@param buffer Buffer
---@return boolean
function Editor:hideBuffer(buffer)
    if not buffer.isShow then
        return false
    end

    self.showBufferArr:remove(buffer)
    buffer:onHide()
    return true
end

---将 Buffer 激活
---@param buffer Buffer
---@return boolean
function Editor:activeBuffer(buffer)
    if buffer.isActive then
        return false
    end

    if self.currentActiveBuffer then
        self:deactiveBuffer(self.currentActiveBuffer)
    end

    self.currentActiveBuffer = buffer
    buffer:onActive()
    return true
end

---将 Buffer 取消激活
---@param buffer Buffer
---@return boolean
function Editor:deactiveBuffer(buffer)
    if not buffer.isActive then
        return false
    end

    buffer:onDeactive()
    self.currentActiveBuffer = nil
    return true
end

function Editor:render()
    ---@param buffer Buffer
    for i, buffer in self.showBufferArr:iparis() do
        buffer:draw()
    end
end

---@private
function Editor:_fileId()
    self.fileId = self.fileId + 1
    return self.fileId
end

---@private
function Editor:_bufferId()
    self.bufferId = self.bufferId + 1
    return self.bufferId
end

return Editor
