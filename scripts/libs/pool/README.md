# Pool.lua

v0.1

Object pool module for lua

## Installation

The pool.lua file should be dropped into an existing project and required by it.

```lua
Pool = require "pool.lua"
```

## Example

```lua
local Pool = require("pool")

local myPool = Pool()
local t = myPool:get()
myPool:recycle(t)
```

Please see [`example.lua`](./example.lua) for more example.

## Usage

TODO

## Test

TODO