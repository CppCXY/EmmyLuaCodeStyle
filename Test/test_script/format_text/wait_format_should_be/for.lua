for i = 1, 20, 30 do
end

for i, v in pairs(i) do print(i, v) end

for k, v in pairs { 1, 2, 3 } do
    for i = 1, 2, 3 do
        print(i, k, v)
    end
end

do  for i = 1, 10 do
        -- body
    end
end
