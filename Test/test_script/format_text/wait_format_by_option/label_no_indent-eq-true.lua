do 
    if true then
        goto hello
    end
    ::hello::
end

function f()
    for i=1,2,3 do
        if true then
            goto continue
        end

        ::continue::
    end
end
