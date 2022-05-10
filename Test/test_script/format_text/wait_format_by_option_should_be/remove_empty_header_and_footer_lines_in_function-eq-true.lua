local f = function()

end

local function f2()
    -- body
end

local function f3()
    -- body
end

local function f4()
    local t = 13
end

local function f5()
    f4(function()
        local d = 13
    end)
end
