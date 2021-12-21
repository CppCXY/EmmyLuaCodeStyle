local t = -1
local t1 = not a and b or c
local t2 = not a .. b or "" - 3 + 4 - 5
local t3 = - -2 + - - - -5

(function() end)()
call(123, 456)(789)
call "12313"(456)
call(123)"456"
print [[
    12313

    456

    789
]]
function f()
    aaaa.bbbb.cccc.callllll()
    aaaa["awfwfw"].bbbbb:cccccc()
    aaaa["wfwfw"]:f()
    aaaa[13131].bbbb.ccc:ddd()
    aaaaa.bbb
        .ccc()
        .eeeee()
        .fffff()
        .jjjjj()

    local t = {
        aaa, bbbb, cccc
    }
    local c = { aaa, 13131, 2424242 }
    local ddd = call { aaaa, bbb, ccc }
    local eee = calll
        { 12313, 35353, fff }
    local fff = function() end
    dd.e().e().aaa = 1231
end
