--- @type string
WORK_DIR = __getWorkDir();
package.path = package.path .. WORK_DIR .. [[/?.lua;]]
package.path = package.path .. WORK_DIR .. [[/scripts/?.lua;]]

function log_error(message)
    print(message)
end

require('main')
