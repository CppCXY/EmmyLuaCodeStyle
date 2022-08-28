local f0 = function()
end

local f1 = function(a)
    return a
end

local f2 = function(a, b)
    return a, b
end

local f3 = function(a, b, c)
    return a, b, c
end

f0()

local a = f1( 123 )
local b1, b2 = f2( 'b1', 'b2' )
local c1, c2, c3 = f3( 'c1', 'c2', 'c3' )

local d1, d2, d3 = f3(
    'c1',
    'c2',
    'c3'
)

