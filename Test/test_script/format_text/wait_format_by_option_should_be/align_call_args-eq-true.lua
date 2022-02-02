print(a, b, c,
      e, f, g)
p(h, i, j, k, l, m, n)

function f()
    h { aaa }
    h {
        cccc
    }
    h(function()
    end)

    h({
        hhhh
    })

    h(function() end,
      function() end)

    h(function()
        local t = 123
    end, aa, bb, ccc, function()
        local d = 456
    end)

    h({

    }, aa, bbb, ccc, {

    })
end
