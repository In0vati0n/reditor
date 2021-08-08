--[[
    File

    @Author In0vati0n
    @Date 2021/08/08
--]]
local Row = require("core.row")
local PathUtils = require("utils.path_utils")

---@class File
local File = Class("File")

---@param id number
---@param path string
function File:init(id, path)
    self.id = id
    self.path = path
    self.filename = ""
    self.isDirty = false

    ---@type Array
    self.rows = Array()

    if path and path ~= "" then
        self.filename = PathUtils.getFileName(path)
        self:_openFile()
    end
end

---@private
---@return boolean
function File:_openFile()
    if PathUtils.fileExists(self.path) then
        for line in io.lines(self.path) do
            self.rows:add(Row:new(line))
        end
        return true
    end
    return false
end

return File
