local t<const> = 123
local t<close> = setmetatable({},{
    __close = function()
    end
}
)

