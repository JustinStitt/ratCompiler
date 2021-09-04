include("fsm.jl")

struct alphabet
    letters::Set{Char}
    digits::Set{Char}
    symbols::Set{Char}
    whitespace::Set{Char}
    alphabet() = new(
        Set([[x for x in 'a':'z']..., 
             [x for x in 'A':'Z']...,
             '_']),
        Set(Char.('0' .+ range(0, 9, step=1))),
        Set([';', '+', '*','/', 
             '\\', '-', ',', '(', ')', 
             '[', ']', '!', '>', '<', 
             '{', '}', '%', '&', '|', '^',
             '=', '.', '?', ':', '\'', '\"']),
        Set([' ', '\n'])
    )
end

function parseInput(q0, inp, Σ)
    for c in inp
        if c in Σ.letters
            q0 = step(q0, Letter(c))
        elseif c in Σ.digits
            q0 = step(q0, Digit(c))
        elseif c in Σ.symbols
            q0 = step(q0, Symbol(c))
        elseif c in Σ.whitespace
            q0 = step(q0, Whitespace())
        else
            println("bad input")
        end
    end
end

test_string = "231+num+foo+3.14;"

parseInput(Start(), test_string, alphabet())

println("Input: ", test_string)

display(store)