
function add(a, b)
    return a + b
end

function multiple(a, b)
    return a * b
end

function getArray()
    return {111, 222, 333}
end

function getTransformedArray()
    array = {111, 222, 333}

    for i=1, 3 do
        array[i] = array[i] * 7
    end

    return array
end