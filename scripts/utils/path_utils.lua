---@class PathUtils
local PathUtils = {}

---从路径中得到文件名
---@param path string
---@return string
function PathUtils:getFileName(path)
    return path:match("^.+/(.+)$")
end

---判断路径文件是否存在
---@param path string
---@return boolean
function PathUtils:fileExists(path)
    local f = io.open(path, "r")
    if f then
        io.close(f)
        return true
    else
        return false
    end
end

return PathUtils
