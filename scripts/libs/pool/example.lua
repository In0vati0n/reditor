-----------------------------
---Define a TestClass
-----------------------------
local TestClass = {}
TestClass.__index = TestClass

setmetatable(
    TestClass,
    {
        __call = function(class, ...)
            local instance = {}
            setmetatable(instance, TestClass)
            instance:new(...)
            return instance
        end
    }
)

function TestClass:new()
    print("create new TestClass instance")
end

function TestClass:awakeFromPool(a)
    print(a)
end

function TestClass:recycleToPool(b)
    print(b)
end

-----------------------------
---Example
-----------------------------

local Pool = require("pool")

local pool =
    Pool(
    {
        ctor = {
            test = TestClass,
            test2 = TestClass,
            test3 = TestClass
        },
        presize = {
            test3 = 10
        }
    }
)

print("----- Example 1 -----")
pool:setCtorHandle(TestClass)
local testClassIns = pool:get("get")
pool:recycle(testClassIns, "recycle")
testClassIns = pool:get("get2")
pool:recycle(testClassIns, "recycle2")

print("----- Example 2 -----")
local testClassIns2 = pool:getWithTag("test", "getWithTag")
pool:recycleWithTag(testClassIns2, "test", "recycleWithTag")
testClassIns2 = pool:getWithTag("test", "getWithTag2")
pool:recycleWithTag(testClassIns2, "test", "recycleWithTag2")

print("----- Example 3 -----")
pool:presize(10, "test2")
local testClassIns3 = pool:getWithTag("test2", "getWithTag")
