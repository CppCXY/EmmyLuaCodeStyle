c["abc"] = 10
c[123] = c["123"]["12345"]
c[ [[a]] ] = c[ [[1]] ]

local t4 = {
    aaa = 1,
    bbbb =  4456,
    ["wgfwwfw" ] = 890,
    [{}] = 131,
    [function() end] = 131,
    [1231] = 12321,
    [ [[123]] ] = 456
}
